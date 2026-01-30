#ifndef XVISUAL_CORE_EXECUTOR_GRAPHEXECUTOR_H
#define XVISUAL_CORE_EXECUTOR_GRAPHEXECUTOR_H

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <stop_token>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "Core/Executor/IEventSink.h"
#include "Core/Executor/INode.h"
#include "Core/Executor/INodeFactory.h"
#include "Core/Executor/NodeState.h"
#include "Core/Executor/ThreadPool.h"
#include "Core/Executor/ThreadSafeQueue.h"
#include "Core/Runtime/ExecutionContext.h"
#include "XGraph/GraphNode.h"
#include "Common/ErrorCode.h"

namespace XVisual {

/**
 * GraphExecutor: core execution engine (no Qt dependency).
 *
 * Phase PR-4 scope:
 * - Parallel execution of independent nodes (DAG with Kahn algorithm).
 * - Thread pool for concurrent node execution.
 * - Node state tracking (Pending/Running/Completed/Failed/Skipped).
 * - Node-boundary cancel (stop_token checked before each node).
 * - Emits Job/Node events through IEventSink.
 */
class GraphExecutor
{
public:
	struct Options
	{
		bool nodeBoundaryCancel = true;
		bool parallelExecution = true;  // PR-4: 启用并行执行
		size_t threadPoolSize = 0;      // 0 = auto (hardware_concurrency - 1)

		explicit Options(bool nodeBoundaryCancel_ = true, bool parallel = true)
			: nodeBoundaryCancel(nodeBoundaryCancel_)
			, parallelExecution(parallel)
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

	/**
	 * 获取节点状态（用于 UI 查询）
	 */
	NodeState getNodeState(const std::string& nodeId) const;

	/**
	 * 获取所有节点状态快照
	 */
	std::unordered_map<std::string, NodeStateInfo> getNodeStates() const;

private:
	void emitEvent(const NodeEvent& e);

	// PR-4: 并行执行相关
	void buildDAG(const std::vector<std::shared_ptr<GraphNode>>& graphSnapshot);
	void runParallel(NodeResolver resolver, Options opt, FinishedCallback onFinished);
	void runSerial(NodeResolver resolver, Options opt, FinishedCallback onFinished);
	void executeNode(const std::string& nodeId, NodeResolver resolver);
	void onNodeComplete(const std::string& nodeId, NodeState finalState, const std::string& errorMsg = "");
	void markDownstreamSkipped(const std::string& nodeId);
	int64_t nowMicros() const;

	std::atomic_bool running_{ false };
	std::atomic_bool canceled_{ false };
	std::atomic_bool failed_{ false };
	std::stop_source stopSource_;
	std::unique_ptr<std::jthread> worker_;

	IEventSink* sink_ = nullptr;

	// for JobStarted/JobFinished
	std::string jobId_;
	std::string graphId_;

	// PR-4: 并行调度数据结构
	std::unique_ptr<ThreadPool> threadPool_;
	ThreadSafeQueue<std::string> readyQueue_;
	std::unordered_map<std::string, int> inDegree_;                          // 节点入度
	std::unordered_map<std::string, std::vector<std::string>> downstream_;   // 下游邻接表
	std::unordered_map<std::string, NodeStateInfo> nodeStates_;              // 节点状态
	std::atomic<int> remaining_{ 0 };                                        // 剩余未完成节点数
	std::atomic<int> completedCount_{ 0 };                                   // 已完成节点数
	int totalNodes_{ 0 };                                                    // 总节点数
	mutable std::mutex stateMutex_;                                          // 保护 nodeStates_

	// 存储图快照供并行执行使用
	std::vector<std::shared_ptr<GraphNode>> graphSnapshot_;
	Options currentOptions_;
};

} // namespace XVisual

#endif // XVISUAL_CORE_EXECUTOR_GRAPHEXECUTOR_H
