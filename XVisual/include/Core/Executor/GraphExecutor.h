#ifndef XVISUAL_CORE_EXECUTOR_GRAPHEXECUTOR_H
#define XVISUAL_CORE_EXECUTOR_GRAPHEXECUTOR_H

#include <atomic>
#include <functional>
#include <memory>
#include <stop_token>
#include <string>
#include <thread>
#include <vector>

#include "Core/Executor/IEventSink.h"
#include "Core/Executor/INode.h"
#include "Core/Executor/INodeFactory.h"
#include "Core/Runtime/ExecutionContext.h"
#include "XGraph/GraphNode.h"
#include "Common/ErrorCode.h"

namespace XVisual {

/**
 * GraphExecutor: core execution engine (no Qt dependency).
 *
 * Phase PR-2 scope:
 * - Serial execution following graph topology (BFS/Kahn order).
 * - Node-boundary cancel (stop_token checked before each node).
 * - Emits Job/Node events through IEventSink.
 */
class GraphExecutor
{
public:
	struct Options
	{
		bool nodeBoundaryCancel;

		explicit Options(bool nodeBoundaryCancel_ = true)
			: nodeBoundaryCancel(nodeBoundaryCancel_)
		{
		}
	};

	using NodeResolver = std::function<INode*(const std::string& nodeId)>;
	using FinishedCallback = std::function<void(XVisual::ErrorCode)>;

	GraphExecutor();
	~GraphExecutor();

	// non-copyable
	GraphExecutor(const GraphExecutor&) = delete;
	GraphExecutor& operator=(const GraphExecutor&) = delete;

	// Starts one execution job. Returns false if already running.
	bool start(std::string graphId,
	           std::vector<std::shared_ptr<GraphNode>> graphSnapshot,
	           NodeResolver resolver,
	           IEventSink* sink,
	           FinishedCallback onFinished = {},
	           Options opt = Options{});

	/**
	 * PR-3 新增: 使用 INodeFactory 在每次执行时创建节点实例,
	 * 实例存储在 ExecutionContext 中, job 结束后自动释放.
	 */
	bool start(std::string graphId,
	           std::vector<std::shared_ptr<GraphNode>> graphSnapshot,
	           INodeFactory* factory,
	           IEventSink* sink,
	           FinishedCallback onFinished = {},
	           Options opt = Options{});

	// Request stop. Effective at node boundary.
	void cancel();

	// Join worker thread if running (non-Qt, safe for MainWindow destructor ordering).
	void wait();

	bool isRunning() const;

private:
	void emitEvent(const NodeEvent& e);

	std::atomic_bool running_{ false };
	std::stop_source stopSource_;
	std::unique_ptr<std::jthread> worker_;

	IEventSink* sink_ = nullptr;

	// for JobStarted/JobFinished
	std::string jobId_;
	std::string graphId_;
};

} // namespace XVisual

#endif // XVISUAL_CORE_EXECUTOR_GRAPHEXECUTOR_H
