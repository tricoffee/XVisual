#include "Core/Executor/GraphExecutor.h"

#include <chrono>
#include <exception>
#include <queue>
#include <unordered_map>

#include "Common/UuidGenerator.h"
#include "Exception/UmapKeyNoFoundException.h"

namespace XVisual {

static std::uint64_t nowUs()
{
	using namespace std::chrono;
	return duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count();
}

GraphExecutor::GraphExecutor() = default;

GraphExecutor::~GraphExecutor()
{
	cancel();
	wait();
	stopSource_ = std::stop_source{};
	running_.store(false);
}

bool GraphExecutor::isRunning() const
{
	return running_.load();
}

void GraphExecutor::cancel()
{
	stopSource_.request_stop();
}

void GraphExecutor::wait()
{
	// Join worker thread (if any). This ensures no further events will be emitted.
	worker_.reset();
}

void GraphExecutor::emitEvent(const NodeEvent& e)
{
	if (sink_)
		sink_->onEvent(e);
}

bool GraphExecutor::start(std::string graphId,
                          std::vector<std::shared_ptr<GraphNode>> graphSnapshot,
                          NodeResolver resolver,
                          IEventSink* sink,
                          FinishedCallback onFinished,
                          Options opt)
{
	if (running_.exchange(true))
		return false;

	// reset stop source per run
	stopSource_ = std::stop_source{};
	sink_ = sink;
	graphId_ = std::move(graphId);
	jobId_ = "Job_" + generateUUID();

	const auto startTs = nowUs();
	emitEvent(NodeEvent{ EventType::JobStarted, jobId_, graphId_, {}, 0, {}, startTs, 0 });

	worker_ = std::make_unique<std::jthread>(
	    [this,
	     graphSnapshot = std::move(graphSnapshot),
	     resolver = std::move(resolver),
	     onFinished = std::move(onFinished),
	     opt,
	     startTs](std::stop_token) mutable
	    {
		    XVisual::ErrorCode result = XVisual::ErrorCode::Success;

		    // Kahn BFS topological execute (serial)
		    std::unordered_map<std::string, int> inDegrees;
		    std::queue<std::shared_ptr<GraphNode>> q;

		    for (const auto& nodePtr : graphSnapshot)
		    {
			    for (const auto& neighbor : nodePtr->neighbors)
			    {
				    auto it = inDegrees.find(neighbor->nodeId);
				    if (it == inDegrees.end())
					    inDegrees[neighbor->nodeId] = 1;
				    else
					    it->second++;
			    }
		    }

		    for (const auto& nodePtr : graphSnapshot)
		    {
			    if (inDegrees[nodePtr->nodeId] == 0)
				    q.push(nodePtr);
		    }

		    auto st = stopSource_.get_token();

		    while (!q.empty())
		    {
			    if (opt.nodeBoundaryCancel && st.stop_requested())
			    {
				    result = XVisual::ErrorCode::Canceled;
				    break;
			    }

			    auto node = q.front();
			    q.pop();
			    const std::string nodeId = node->nodeId;

			    const auto nodeStart = nowUs();
			    emitEvent(NodeEvent{ EventType::NodeStarted, jobId_, graphId_, nodeId, 0, {}, nodeStart, 0 });

			    try
			    {
				    INode* execNode = resolver ? resolver(nodeId) : nullptr;
				    if (!execNode)
				    {
					    result = XVisual::ErrorCode::UmapKeyNoFound;
					    emitEvent(NodeEvent{ EventType::NodeFinished,
					                         jobId_,
					                         graphId_,
					                         nodeId,
					                         static_cast<int>(result),
					                         "resolver returned nullptr",
					                         nowUs(),
					                         nowUs() - nodeStart });
					    break;
				    }

				    execNode->initOperands();
				    execNode->run(st);

				    emitEvent(NodeEvent{ EventType::NodeFinished, jobId_, graphId_, nodeId, 0, {}, nowUs(), nowUs() - nodeStart });
			    }
			    catch (UmapKeyNoFoundException& e)
			    {
				    result = XVisual::ErrorCode::UmapKeyNoFound;
				    emitEvent(NodeEvent{ EventType::NodeFinished,
				                         jobId_,
				                         graphId_,
				                         nodeId,
				                         static_cast<int>(result),
				                         e.what(),
				                         nowUs(),
				                         nowUs() - nodeStart });
				    break;
			    }
			    catch (std::exception& e)
			    {
				    // Reuse existing error code to avoid expanding ErrorCode in PR-2
				    result = XVisual::ErrorCode::UnImplementedError;
				    emitEvent(NodeEvent{ EventType::NodeFinished,
				                         jobId_,
				                         graphId_,
				                         nodeId,
				                         static_cast<int>(result),
				                         e.what(),
				                         nowUs(),
				                         nowUs() - nodeStart });
				    break;
			    }

			    for (const auto& neighbor : node->neighbors)
			    {
				    inDegrees[neighbor->nodeId]--;
				    if (inDegrees[neighbor->nodeId] == 0)
					    q.push(neighbor);
			    }
		    }

		    emitEvent(NodeEvent{ EventType::JobFinished,
		                         jobId_,
		                         graphId_,
		                         {},
		                         static_cast<int>(result),
		                         XVisual::errorCodeToStr(result),
		                         nowUs(),
		                         nowUs() - startTs });

		    running_.store(false);
		    if (onFinished)
			    onFinished(result);
	    });

	return true;
}

} // namespace XVisual
