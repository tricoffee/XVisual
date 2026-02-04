#include "Core/Device/DeviceQueueManager.h"
#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"

namespace XVisual {

DeviceQueueManager::DeviceQueueManager()
    : shutdown_(false)
{
    XLOG_INFO("DeviceQueueManager: Initializing...", CURRENT_THREAD_ID);
    
    // 创建 CPU 队列（多 worker 线程池）
    cpuQueue_ = std::make_unique<CPUQueue>();
    
    // 创建 GPU 队列（单 worker 串行队列，占位）
    gpuQueue_ = std::make_unique<SerialQueue>(DeviceType::GPU, "GPUQueue");
    
    // 创建 NPU 队列（单 worker 串行队列，占位）
    npuQueue_ = std::make_unique<SerialQueue>(DeviceType::NPU, "NPUQueue");
    
    XLOG_INFO("DeviceQueueManager: Initialized (CPU + GPU + NPU queues)", CURRENT_THREAD_ID);
}

DeviceQueueManager::~DeviceQueueManager()
{
    // 确保 shutdown 被调用
    if (!shutdown_) {
        shutdown();
    }
}

IDeviceQueue* DeviceQueueManager::getQueue(DeviceType type)
{
    switch (type) {
        case DeviceType::CPU:
            return cpuQueue_.get();
        case DeviceType::GPU:
            return gpuQueue_.get();
        case DeviceType::NPU:
            return npuQueue_.get();
        case DeviceType::Any:
            // PR-5.1 约定：Any 应由调用方映射，这里作为 fallback 返回 CPU
            XLOG_INFO("DeviceQueueManager::getQueue: Any mapped to CPU (caller should handle)", 
                      CURRENT_THREAD_ID);
            return cpuQueue_.get();
        default:
            // 未知类型，返回 CPU 作为 fallback
            XLOG_INFO("DeviceQueueManager::getQueue: Unknown type, fallback to CPU", 
                      CURRENT_THREAD_ID);
            return cpuQueue_.get();
    }
}

void DeviceQueueManager::shutdown()
{
    if (shutdown_) {
        return;  // 幂等
    }
    
    XLOG_INFO("DeviceQueueManager::shutdown: Starting...", CURRENT_THREAD_ID);
    
    shutdown_ = true;
    
    // 按顺序关闭队列
    // 注意：这里的顺序可以根据实际需求调整
    // 先关 CPU（可能有大量任务），再关 GPU/NPU
    
    if (cpuQueue_) {
        cpuQueue_->shutdown();
    }
    
    if (gpuQueue_) {
        gpuQueue_->shutdown();
    }
    
    if (npuQueue_) {
        npuQueue_->shutdown();
    }
    
    XLOG_INFO("DeviceQueueManager::shutdown: Completed", CURRENT_THREAD_ID);
}

} // namespace XVisual
