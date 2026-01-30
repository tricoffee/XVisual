#ifndef XVISUAL_CORE_EXECUTOR_NODESTATE_H
#define XVISUAL_CORE_EXECUTOR_NODESTATE_H

#include <cstdint>
#include <string>

namespace XVisual {

/**
 * NodeState: 节点执行状态枚举
 */
enum class NodeState : uint8_t {
    Pending,    // 尚未就绪（有未完成的上游依赖）
    Ready,      // 就绪，等待线程池调度
    Running,    // 正在执行
    Completed,  // 执行成功
    Failed,     // 执行失败
    Canceled,   // 被取消（用户主动取消）
    Skipped     // 被跳过（上游失败导致）
};

/**
 * NodeStateInfo: 节点状态详细信息
 */
struct NodeStateInfo {
    NodeState state = NodeState::Pending;
    int64_t startTime = 0;      // 微秒时间戳（执行开始）
    int64_t endTime = 0;        // 微秒时间戳（执行结束）
    int64_t duration = 0;       // 执行耗时（微秒）
    std::string errorMessage;   // 失败时的错误信息

    NodeStateInfo() = default;
    explicit NodeStateInfo(NodeState s) : state(s) {}
};

/**
 * 将 NodeState 转换为字符串（用于日志和调试）
 */
inline const char* nodeStateToString(NodeState state)
{
    switch (state)
    {
        case NodeState::Pending:   return "Pending";
        case NodeState::Ready:     return "Ready";
        case NodeState::Running:   return "Running";
        case NodeState::Completed: return "Completed";
        case NodeState::Failed:    return "Failed";
        case NodeState::Canceled:  return "Canceled";
        case NodeState::Skipped:   return "Skipped";
        default:                   return "Unknown";
    }
}

} // namespace XVisual

#endif // XVISUAL_CORE_EXECUTOR_NODESTATE_H
