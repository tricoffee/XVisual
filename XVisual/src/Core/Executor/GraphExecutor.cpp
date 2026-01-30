#include "Core/Executor/GraphExecutor.h"

#include <chrono>
#include <exception>
#include <queue>

#include "Common/LoggerInstance.h"
#include "Common/UuidGenerator.h"
#include "Common/XThreadMacro.h"
#include "Exception/UmapKeyNoFoundException.h"

namespace XVisual {

// ============================================================================
// Helper: 获取当前时间戳（微秒）
// ============================================================================
static std::uint64_t nowUs()
{
	using namespace std::chrono;
	return duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count();
}

// ============================================================================
// 构造 / 析构
// ============================================================================
GraphExecutor::GraphExecutor() = default;

GraphExecutor::~GraphExecutor()
{
	cancel();
	wait();
	if (threadPool_)
	{
		threadPool_->shutdown();
		threadPool_.reset();
	}
	stopSource_ = std::stop_source{};
	running_.store(false);
}

bool GraphExecutor::isRunning() const
{
	return running_.load();
}

void GraphExecutor::cancel()
{
	canceled_.store(true);
	stopSource_.request_stop();
	readyQueue_.stop();
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

int64_t GraphExecutor::nowMicros() const
{
	return static_cast<int64_t>(nowUs());
}

// ============================================================================
// PR-4.5b: 心跳线程
// ============================================================================
void GraphExecutor::startHeartbeatThread()
{
	if (!currentOptions_.enableHeartbeat)
		return;

	// 创建心跳线程
	heartbeatThread_ = std::make_unique<std::jthread>(
		[this](std::stop_token st) {
			heartbeatLoop(st);
		}
	);
}

void GraphExecutor::stopHeartbeatThread()
{
	if (heartbeatThread_)
	{
		heartbeatThread_->request_stop();
		heartbeatThread_->join();
		heartbeatThread_.reset();
	}
}

void GraphExecutor::heartbeatLoop(std::stop_token st)
{
	while (!st.stop_requested() && remaining_.load() > 0)
	{
		// 等待心跳间隔
		std::this_thread::sleep_for(currentOptions_.heartbeatInterval);

		// 检查是否应该停止
		if (st.stop_requested() || remaining_.load() <= 0)
			break;

		// ✅ 优化：锁内只做快照拷贝，避免持锁期间 emitEvent
		// 这样心跳线程对 worker 的影响趋近于 0
		std::vector<std::pair<std::string, std::uint64_t>> runningNodes;
		{
			std::lock_guard<std::mutex> lock(stateMutex_);
			int64_t now = nowMicros();
			for (const auto& [nodeId, stateInfo] : nodeStates_)
			{
				if (stateInfo.state == NodeState::Running)
				{
					runningNodes.emplace_back(
						nodeId,
						static_cast<std::uint64_t>(now - stateInfo.startTime)
					);
				}
			}
		}
		// ✅ 锁已释放，锁外 emitEvent
		
		for (const auto& [nodeId, elapsedUs] : runningNodes)
		{
			NodeEvent evt;
			evt.type = EventType::NodeHeartbeat;
			evt.jobId = jobId_;
			evt.graphId = graphId_;
			evt.nodeId = nodeId;
			evt.state = NodeState::Running;
			evt.elapsedUs = elapsedUs;
			evt.tsUs = static_cast<std::uint64_t>(nowMicros());
			emitEvent(evt);
		}
	}
}

// ============================================================================
// 获取节点状态
// ============================================================================
NodeState GraphExecutor::getNodeState(const std::string& nodeId) const
{
	std::lock_guard<std::mutex> lock(stateMutex_);
	auto it = nodeStates_.find(nodeId);
	if (it != nodeStates_.end())
		return it->second.state;
	return NodeState::Pending;
}

std::unordered_map<std::string, NodeStateInfo> GraphExecutor::getNodeStates() const
{
	std::lock_guard<std::mutex> lock(stateMutex_);
	return nodeStates_;
}

// ============================================================================
// 构建 DAG（计算入度和下游邻接表）
// ============================================================================
void GraphExecutor::buildDAG(const std::vector<std::shared_ptr<GraphNode>>& graphSnapshot)
{
	inDegree_.clear();
	downstream_.clear();
	nodeStates_.clear();

	// 初始化所有节点入度为 0，状态为 Pending
	for (const auto& nodePtr : graphSnapshot)
	{
		inDegree_[nodePtr->nodeId] = 0;
		nodeStates_[nodePtr->nodeId] = NodeStateInfo(NodeState::Pending);
	}

	// 计算入度和下游邻接表
	// GraphNode::neighbors 表示当前节点的下游节点
	for (const auto& nodePtr : graphSnapshot)
	{
		const std::string& fromId = nodePtr->nodeId;
		for (const auto& neighbor : nodePtr->neighbors)
		{
			const std::string& toId = neighbor->nodeId;
			// fromId -> toId 边，toId 的入度 +1
			inDegree_[toId]++;
			// fromId 的下游是 toId
			downstream_[fromId].push_back(toId);
		}
	}

	// DEBUG: 打印每个节点的入度
	for (const auto& nodePtr : graphSnapshot)
	{
		XLOG_INFO("buildDAG: node=" + nodePtr->nodeId + 
		          " className=" + nodePtr->className +
		          " inDegree=" + std::to_string(inDegree_[nodePtr->nodeId]) +
		          " neighbors=" + std::to_string(nodePtr->neighbors.size()),
		          CURRENT_THREAD_ID);
	}

	totalNodes_ = static_cast<int>(graphSnapshot.size());
	remaining_.store(totalNodes_);
	completedCount_.store(0);
}

// ============================================================================
// 标记下游节点为 Skipped
// ============================================================================
void GraphExecutor::markDownstreamSkipped(const std::string& nodeId)
{
	std::queue<std::string> toSkip;
	toSkip.push(nodeId);

	while (!toSkip.empty())
	{
		std::string current = toSkip.front();
		toSkip.pop();

		auto itDownstream = downstream_.find(current);
		if (itDownstream == downstream_.end())
			continue;

		for (const auto& downId : itDownstream->second)
		{
			std::lock_guard<std::mutex> lock(stateMutex_);
			auto& stateInfo = nodeStates_[downId];
			if (stateInfo.state == NodeState::Pending || stateInfo.state == NodeState::Ready)
			{
				stateInfo.state = NodeState::Skipped;
				stateInfo.endTime = nowMicros();

				// 发送 NodeSkipped 事件
				NodeEvent evt;
				evt.type = EventType::NodeSkipped;
				evt.jobId = jobId_;
				evt.graphId = graphId_;
				evt.nodeId = downId;
				evt.state = NodeState::Skipped;
				evt.tsUs = static_cast<std::uint64_t>(stateInfo.endTime);
				evt.message = "Upstream failed";
				emitEvent(evt);

				remaining_.fetch_sub(1);
				toSkip.push(downId);
			}
		}
	}
}

// ============================================================================
// 节点完成回调
// ============================================================================
void GraphExecutor::onNodeComplete(const std::string& nodeId, NodeState finalState, const std::string& errorMsg)
{
	{
		std::lock_guard<std::mutex> lock(stateMutex_);
		auto& stateInfo = nodeStates_[nodeId];
		stateInfo.state = finalState;
		stateInfo.endTime = nowMicros();
		stateInfo.duration = stateInfo.endTime - stateInfo.startTime;
		if (!errorMsg.empty())
			stateInfo.errorMessage = errorMsg;
	}

	if (finalState == NodeState::Failed)
	{
		failed_.store(true);
		// 标记所有下游为 Skipped
		markDownstreamSkipped(nodeId);
	}
	else if (finalState == NodeState::Completed)
	{
		completedCount_.fetch_add(1);

		// 更新下游入度，将就绪的节点推入队列
		auto itDownstream = downstream_.find(nodeId);
		if (itDownstream != downstream_.end())
		{
			XLOG_INFO("onNodeComplete: node=" + nodeId + " has " + 
			          std::to_string(itDownstream->second.size()) + " downstream nodes",
			          CURRENT_THREAD_ID);
			for (const auto& downId : itDownstream->second)
			{
				int oldDegree = inDegree_[downId];
				int newDegree = --inDegree_[downId];
				XLOG_INFO("onNodeComplete: --inDegree[" + downId + "]: " + 
				          std::to_string(oldDegree) + " -> " + std::to_string(newDegree),
				          CURRENT_THREAD_ID);
				if (newDegree == 0)
				{
					if (!failed_.load() && !canceled_.load())
					{
						{
							std::lock_guard<std::mutex> lock(stateMutex_);
							nodeStates_[downId].state = NodeState::Ready;
						}
						readyQueue_.push(downId);
					}
					else
					{
						// 如果已经失败或取消，标记为 Skipped/Canceled
						std::lock_guard<std::mutex> lock(stateMutex_);
						auto& stateInfo = nodeStates_[downId];
						stateInfo.state = canceled_.load() ? NodeState::Canceled : NodeState::Skipped;
						stateInfo.endTime = nowMicros();
						remaining_.fetch_sub(1);
					}
				}
			}
		}
	}

	int remainingAfter = remaining_.fetch_sub(1) - 1;

	// 如果所有节点都完成了，停止就绪队列以唤醒主调度循环
	if (remainingAfter <= 0)
	{
		readyQueue_.stop();
	}

	// 发送进度更新事件
	NodeEvent progressEvt;
	progressEvt.type = EventType::ProgressUpdate;
	progressEvt.jobId = jobId_;
	progressEvt.graphId = graphId_;
	progressEvt.completedNodes = completedCount_.load();
	progressEvt.totalNodes = totalNodes_;
	progressEvt.progress = (totalNodes_ > 0) ? (completedCount_.load() * 100 / totalNodes_) : 100;
	progressEvt.tsUs = static_cast<std::uint64_t>(nowMicros());
	emitEvent(progressEvt);
}

// ============================================================================
// 执行单个节点
// ============================================================================
void GraphExecutor::executeNode(const std::string& nodeId, NodeResolver resolver)
{
	// 检查是否已取消
	if (canceled_.load())
	{
		std::lock_guard<std::mutex> lock(stateMutex_);
		auto& stateInfo = nodeStates_[nodeId];
		stateInfo.state = NodeState::Canceled;
		stateInfo.endTime = nowMicros();
		remaining_.fetch_sub(1);
		return;
	}

	// 标记为 Running
	int64_t startTime = nowMicros();
	{
		std::lock_guard<std::mutex> lock(stateMutex_);
		auto& stateInfo = nodeStates_[nodeId];
		stateInfo.state = NodeState::Running;
		stateInfo.startTime = startTime;
	}

	// 发送 NodeStarted 事件
	NodeEvent startEvt;
	startEvt.type = EventType::NodeStarted;
	startEvt.jobId = jobId_;
	startEvt.graphId = graphId_;
	startEvt.nodeId = nodeId;
	startEvt.state = NodeState::Running;
	startEvt.tsUs = static_cast<std::uint64_t>(startTime);
	emitEvent(startEvt);

	// 执行节点
	NodeState finalState = NodeState::Completed;
	std::string errorMsg;
	int errorCode = 0;

	try
	{
		INode* execNode = resolver ? resolver(nodeId) : nullptr;
		if (!execNode)
		{
			finalState = NodeState::Failed;
			errorMsg = "resolver returned nullptr";
			errorCode = static_cast<int>(XVisual::ErrorCode::UmapKeyNoFound);
		}
		else
		{
			execNode->execute(stopSource_.get_token());
			finalState = NodeState::Completed;
		}
	}
	catch (UmapKeyNoFoundException& e)
	{
		finalState = NodeState::Failed;
		errorMsg = e.what();
		errorCode = static_cast<int>(XVisual::ErrorCode::UmapKeyNoFound);
	}
	catch (std::exception& e)
	{
		finalState = NodeState::Failed;
		errorMsg = e.what();
		errorCode = static_cast<int>(XVisual::ErrorCode::UnImplementedError);
	}

	// 发送 NodeFinished 事件
	int64_t endTime = nowMicros();
	NodeEvent finishEvt;
	finishEvt.type = EventType::NodeFinished;
	finishEvt.jobId = jobId_;
	finishEvt.graphId = graphId_;
	finishEvt.nodeId = nodeId;
	finishEvt.state = finalState;
	finishEvt.code = errorCode;
	finishEvt.message = errorMsg;
	finishEvt.tsUs = static_cast<std::uint64_t>(endTime);
	finishEvt.durationUs = static_cast<std::uint64_t>(endTime - startTime);
	emitEvent(finishEvt);

	// 通知完成
	onNodeComplete(nodeId, finalState, errorMsg);
}

// ============================================================================
// 并行执行
// ============================================================================
void GraphExecutor::runParallel(NodeResolver resolver, Options opt, FinishedCallback onFinished)
{
	const auto startTs = nowUs();

	// 创建线程池
	size_t poolSize = opt.threadPoolSize;
	if (poolSize == 0)
		poolSize = std::max(1u, std::thread::hardware_concurrency() - 1);
	threadPool_ = std::make_unique<ThreadPool>(poolSize);

	// PR-4.5b: 启动心跳线程
	startHeartbeatThread();

	// 将所有入度为 0 的节点推入就绪队列
	for (const auto& nodePtr : graphSnapshot_)
	{
		int degree = inDegree_[nodePtr->nodeId];
		XLOG_INFO("runParallel: checking node=" + nodePtr->nodeId + " inDegree=" + std::to_string(degree),
		          CURRENT_THREAD_ID);
		if (degree == 0)
		{
			XLOG_INFO("runParallel: pushing READY node=" + nodePtr->nodeId, CURRENT_THREAD_ID);
			{
				std::lock_guard<std::mutex> lock(stateMutex_);
				nodeStates_[nodePtr->nodeId].state = NodeState::Ready;
			}
			readyQueue_.push(nodePtr->nodeId);
		}
	}

	// 主调度循环
	while (remaining_.load() > 0 && !canceled_.load())
	{
		auto nodeIdOpt = readyQueue_.pop();
		if (!nodeIdOpt.has_value())
		{
			// 队列停止或为空，检查是否还有任务在执行
			if (remaining_.load() <= 0)
				break;
			// 短暂等待后重试
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;
		}

		const std::string nodeId = *nodeIdOpt;

		// 提交到线程池执行
		threadPool_->submit([this, nodeId, resolver]() {
			executeNode(nodeId, resolver);
		});
	}

	// 等待所有任务完成
	threadPool_->waitAll();

	// PR-4.5b: 停止心跳线程
	stopHeartbeatThread();

	// 确定最终结果
	XVisual::ErrorCode result = XVisual::ErrorCode::Success;
	if (canceled_.load())
	{
		result = XVisual::ErrorCode::Canceled;
	}
	else if (failed_.load())
	{
		result = XVisual::ErrorCode::UnImplementedError; // 表示有节点失败
	}

	// 发送 JobFinished 事件
	NodeEvent finishEvt;
	finishEvt.type = EventType::JobFinished;
	finishEvt.jobId = jobId_;
	finishEvt.graphId = graphId_;
	finishEvt.code = static_cast<int>(result);
	finishEvt.message = XVisual::errorCodeToStr(result);
	finishEvt.tsUs = nowUs();
	finishEvt.durationUs = nowUs() - startTs;
	finishEvt.completedNodes = completedCount_.load();
	finishEvt.totalNodes = totalNodes_;
	emitEvent(finishEvt);

	// 清理
	threadPool_->shutdown();
	threadPool_.reset();

	running_.store(false);
	if (onFinished)
		onFinished(result);
}

// ============================================================================
// 串行执行（保留原有逻辑，作为后备方案）
// ============================================================================
void GraphExecutor::runSerial(NodeResolver resolver, Options opt, FinishedCallback onFinished)
{
	const auto startTs = nowUs();
	XVisual::ErrorCode result = XVisual::ErrorCode::Success;

	std::queue<std::string> q;

	// 将所有入度为 0 的节点推入队列
	for (const auto& nodePtr : graphSnapshot_)
	{
		if (inDegree_[nodePtr->nodeId] == 0)
			q.push(nodePtr->nodeId);
	}

	auto st = stopSource_.get_token();

	while (!q.empty())
	{
		if (opt.nodeBoundaryCancel && st.stop_requested())
		{
			result = XVisual::ErrorCode::Canceled;
			break;
		}

		std::string nodeId = q.front();
		q.pop();

		const auto nodeStart = nowUs();

		// 发送 NodeStarted 事件
		{
			std::lock_guard<std::mutex> lock(stateMutex_);
			nodeStates_[nodeId].state = NodeState::Running;
			nodeStates_[nodeId].startTime = static_cast<int64_t>(nodeStart);
		}

		NodeEvent startEvt;
		startEvt.type = EventType::NodeStarted;
		startEvt.jobId = jobId_;
		startEvt.graphId = graphId_;
		startEvt.nodeId = nodeId;
		startEvt.state = NodeState::Running;
		startEvt.tsUs = nodeStart;
		emitEvent(startEvt);

		NodeState finalState = NodeState::Completed;
		std::string errorMsg;
		int errorCode = 0;

		try
		{
			INode* execNode = resolver ? resolver(nodeId) : nullptr;
			if (!execNode)
			{
				result = XVisual::ErrorCode::UmapKeyNoFound;
				finalState = NodeState::Failed;
				errorMsg = "resolver returned nullptr";
				errorCode = static_cast<int>(result);

				NodeEvent finEvt;
				finEvt.type = EventType::NodeFinished;
				finEvt.jobId = jobId_;
				finEvt.graphId = graphId_;
				finEvt.nodeId = nodeId;
				finEvt.state = finalState;
				finEvt.code = errorCode;
				finEvt.message = errorMsg;
				finEvt.tsUs = nowUs();
				finEvt.durationUs = nowUs() - nodeStart;
				emitEvent(finEvt);
				break;
			}

			execNode->execute(st);
			finalState = NodeState::Completed;
		}
		catch (UmapKeyNoFoundException& e)
		{
			result = XVisual::ErrorCode::UmapKeyNoFound;
			finalState = NodeState::Failed;
			errorMsg = e.what();
			errorCode = static_cast<int>(result);
		}
		catch (std::exception& e)
		{
			result = XVisual::ErrorCode::UnImplementedError;
			finalState = NodeState::Failed;
			errorMsg = e.what();
			errorCode = static_cast<int>(result);
		}

		// 更新状态
		{
			std::lock_guard<std::mutex> lock(stateMutex_);
			auto& stateInfo = nodeStates_[nodeId];
			stateInfo.state = finalState;
			stateInfo.endTime = static_cast<int64_t>(nowUs());
			stateInfo.duration = stateInfo.endTime - stateInfo.startTime;
			if (!errorMsg.empty())
				stateInfo.errorMessage = errorMsg;
		}

		// 发送 NodeFinished 事件
		NodeEvent finEvt;
		finEvt.type = EventType::NodeFinished;
		finEvt.jobId = jobId_;
		finEvt.graphId = graphId_;
		finEvt.nodeId = nodeId;
		finEvt.state = finalState;
		finEvt.code = errorCode;
		finEvt.message = errorMsg;
		finEvt.tsUs = nowUs();
		finEvt.durationUs = nowUs() - nodeStart;
		emitEvent(finEvt);

		if (finalState == NodeState::Failed)
			break;

		completedCount_.fetch_add(1);

		// 更新下游入度
		auto itDownstream = downstream_.find(nodeId);
		if (itDownstream != downstream_.end())
		{
			for (const auto& downId : itDownstream->second)
			{
				if (--inDegree_[downId] == 0)
					q.push(downId);
			}
		}
	}

	// 发送 JobFinished 事件
	NodeEvent jobFinEvt;
	jobFinEvt.type = EventType::JobFinished;
	jobFinEvt.jobId = jobId_;
	jobFinEvt.graphId = graphId_;
	jobFinEvt.code = static_cast<int>(result);
	jobFinEvt.message = XVisual::errorCodeToStr(result);
	jobFinEvt.tsUs = nowUs();
	jobFinEvt.durationUs = nowUs() - startTs;
	jobFinEvt.completedNodes = completedCount_.load();
	jobFinEvt.totalNodes = totalNodes_;
	emitEvent(jobFinEvt);

	running_.store(false);
	if (onFinished)
		onFinished(result);
}

// ============================================================================
// start() 使用 NodeResolver
// ============================================================================
bool GraphExecutor::start(std::string graphId,
                          std::vector<std::shared_ptr<GraphNode>> graphSnapshot,
                          NodeResolver resolver,
                          IEventSink* sink,
                          FinishedCallback onFinished,
                          Options opt)
{
	if (running_.exchange(true))
		return false;

	// 重置状态
	stopSource_ = std::stop_source{};
	canceled_.store(false);
	failed_.store(false);
	readyQueue_.reset();

	sink_ = sink;
	graphId_ = std::move(graphId);
	jobId_ = "Job_" + generateUUID();
	graphSnapshot_ = std::move(graphSnapshot);
	currentOptions_ = opt;

	// 构建 DAG
	buildDAG(graphSnapshot_);

	const auto startTs = nowUs();

	// 发送 JobStarted 事件
	NodeEvent startEvt;
	startEvt.type = EventType::JobStarted;
	startEvt.jobId = jobId_;
	startEvt.graphId = graphId_;
	startEvt.totalNodes = totalNodes_;
	startEvt.tsUs = startTs;
	emitEvent(startEvt);

	// 在工作线程中执行
	worker_ = std::make_unique<std::jthread>(
	    [this,
	     resolver = std::move(resolver),
	     onFinished = std::move(onFinished),
	     opt](std::stop_token) mutable
	    {
		    if (opt.parallelExecution)
		    {
			    runParallel(std::move(resolver), opt, std::move(onFinished));
		    }
		    else
		    {
			    runSerial(std::move(resolver), opt, std::move(onFinished));
		    }
	    });

	return true;
}

// ============================================================================
// start() 使用 INodeFactory (PR-3)
// ============================================================================
bool GraphExecutor::start(std::string graphId,
                          std::vector<std::shared_ptr<GraphNode>> graphSnapshot,
                          INodeFactory* factory,
                          IEventSink* sink,
                          FinishedCallback onFinished,
                          Options opt)
{
	if (running_.exchange(true))
		return false;

	// 重置状态
	stopSource_ = std::stop_source{};
	canceled_.store(false);
	failed_.store(false);
	readyQueue_.reset();

	sink_ = sink;
	graphId_ = std::move(graphId);
	jobId_ = "Job_" + generateUUID();
	graphSnapshot_ = std::move(graphSnapshot);
	currentOptions_ = opt;

	// 构建 DAG
	buildDAG(graphSnapshot_);

	const auto startTs = nowUs();

	// 发送 JobStarted 事件
	NodeEvent startEvt;
	startEvt.type = EventType::JobStarted;
	startEvt.jobId = jobId_;
	startEvt.graphId = graphId_;
	startEvt.totalNodes = totalNodes_;
	startEvt.tsUs = startTs;
	emitEvent(startEvt);

	// 在工作线程中执行
	worker_ = std::make_unique<std::jthread>(
	    [this,
	     factory,
	     onFinished = std::move(onFinished),
	     opt](std::stop_token) mutable
	    {
		    // Create ExecutionContext (holds created nodes lifetime)
		    ExecutionContext ctx;
		    ctx.st = stopSource_.get_token();

		    // 创建 resolver：从 factory 创建或复用节点
		    NodeResolver resolver = [this, factory, &ctx](const std::string& nodeId) -> INode*
		    {
			    auto itNode = ctx.nodes.find(nodeId);
			    if (itNode != ctx.nodes.end())
				    return itNode->second.get();

			    // 查找 className
			    std::string className;
			    for (const auto& nodePtr : graphSnapshot_)
			    {
				    if (nodePtr->nodeId == nodeId)
				    {
					    className = nodePtr->className;
					    break;
				    }
			    }

			    NodeConfig cfg;
			    cfg.nodeId = nodeId;
			    cfg.className = className;
			    auto created = factory->createNode(cfg);
			    if (!created)
				    return nullptr;

			    INode* rawPtr = created.get();
			    ctx.nodes.emplace(nodeId, std::move(created));
			    return rawPtr;
		    };

		    if (opt.parallelExecution)
		    {
			    runParallel(resolver, opt, std::move(onFinished));
		    }
		    else
		    {
			    runSerial(resolver, opt, std::move(onFinished));
		    }

		    // ctx 销毁时自动释放所有节点
	    });

	return true;
}

} // namespace XVisual
