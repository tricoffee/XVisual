#ifndef XVISUAL_CORE_EXECUTOR_IEVENTSINK_H
#define XVISUAL_CORE_EXECUTOR_IEVENTSINK_H

#include <cstdint>
#include <string>
#include "NodeState.h"

namespace XVisual {

enum class EventType
{
	JobStarted,
	NodeStarted,
	NodeFinished,
	NodeSkipped,    // 新增：节点被跳过（上游失败）
	JobFinished,
	ProgressUpdate, // 新增：进度更新
	NodeHeartbeat,  // PR-4.5b: 长节点心跳（表示节点仍在运行）
	Log
};

struct NodeEvent
{
	EventType type = EventType::Log;

	// identifiers
	std::string jobId;
	std::string graphId;
	std::string nodeId;

	// status / payload
	int code = 0; // Typically XVisual::ErrorCode cast to int
	std::string message;
	NodeState state = NodeState::Pending; // 新增：节点状态

	// timing
	std::uint64_t tsUs = 0;
	std::uint64_t durationUs = 0;
	std::uint64_t elapsedUs = 0;  // PR-4.5b: 节点已运行时长（心跳用）

	// progress (for ProgressUpdate event)
	int progress = 0;       // 0-100
	int totalNodes = 0;     // 总节点数
	int completedNodes = 0; // 已完成节点数
};

class IEventSink
{
public:
	virtual ~IEventSink() = default;
	virtual void onEvent(const NodeEvent& e) = 0;
};

} // namespace XVisual

#endif // XVISUAL_CORE_EXECUTOR_IEVENTSINK_H
