#include "Core/Device/DeviceRuntime.h"
#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include <stdexcept>

namespace XVisual {

DeviceRuntime::DeviceRuntime(const DeviceRuntimeConfig& config)
    : config_(config)
    , shutdown_(false)
{
    XLOG_INFO("DeviceRuntime: Initializing...", CURRENT_THREAD_ID);
    
    // 1. 创建 CPU 队列（共享一个线程池）
    cpuQueue_ = std::make_unique<CPUQueue>();
    XLOG_INFO("DeviceRuntime: Created cpu:0 queue (CPUQueue/ThreadPool)", CURRENT_THREAD_ID);
    
    // 2. 创建 GPU 队列（每实例一个 SerialQueue）
    for (int i = 0; i < config_.gpuCount; ++i)
    {
        std::string queueName = "gpu:" + std::to_string(i);
        gpuQueues_.push_back(std::make_unique<SerialQueue>(DeviceType::GPU, queueName));
        XLOG_INFO("DeviceRuntime: Created " + queueName + " queue (SerialQueue)", CURRENT_THREAD_ID);
    }
    
    // 3. 创建 NPU 队列（每实例一个 SerialQueue）
    for (int i = 0; i < config_.npuCount; ++i)
    {
        std::string queueName = "npu:" + std::to_string(i);
        npuQueues_.push_back(std::make_unique<SerialQueue>(DeviceType::NPU, queueName));
        XLOG_INFO("DeviceRuntime: Created " + queueName + " queue (SerialQueue)", CURRENT_THREAD_ID);
    }
    
    XLOG_INFO("DeviceRuntime: Initialized (1 CPU, " + 
              std::to_string(config_.gpuCount) + " GPU, " + 
              std::to_string(config_.npuCount) + " NPU)", CURRENT_THREAD_ID);
}

DeviceRuntime::~DeviceRuntime()
{
    if (!shutdown_)
    {
        shutdown();
    }
}

std::vector<DeviceInstance> DeviceRuntime::list(DeviceType type) const
{
    std::vector<DeviceInstance> result;
    
    switch (type)
    {
    case DeviceType::CPU:
        result.push_back(DeviceInstance::CPU(0));
        break;
        
    case DeviceType::GPU:
        for (int i = 0; i < config_.gpuCount; ++i)
        {
            result.push_back(DeviceInstance::GPU(i));
        }
        break;
        
    case DeviceType::NPU:
        for (int i = 0; i < config_.npuCount; ++i)
        {
            result.push_back(DeviceInstance::NPU(i));
        }
        break;
        
    case DeviceType::Any:
        // Any 不是具体类型，返回空
        break;
        
    default:
        break;
    }
    
    return result;
}

IDeviceQueue& DeviceRuntime::queue(const DeviceInstance& dev)
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    switch (dev.type)
    {
    case DeviceType::CPU:
        if (dev.index == 0 && cpuQueue_)
        {
            return *cpuQueue_;
        }
        break;
        
    case DeviceType::GPU:
        if (dev.index >= 0 && dev.index < static_cast<int>(gpuQueues_.size()))
        {
            return *gpuQueues_[dev.index];
        }
        break;
        
    case DeviceType::NPU:
        if (dev.index >= 0 && dev.index < static_cast<int>(npuQueues_.size()))
        {
            return *npuQueues_[dev.index];
        }
        break;
        
    case DeviceType::Any:
        // Any 映射到 CPU:0（由 policy 处理，这里作为 fallback）
        if (cpuQueue_)
        {
            XLOG_INFO("DeviceRuntime::queue: Any mapped to cpu:0 (fallback)", CURRENT_THREAD_ID);
            return *cpuQueue_;
        }
        break;
        
    default:
        break;
    }
    
    throw std::out_of_range("DeviceRuntime::queue: Device instance not found: " + dev.name());
}

bool DeviceRuntime::hasDevice(const DeviceInstance& dev) const
{
    switch (dev.type)
    {
    case DeviceType::CPU:
        return dev.index == 0;
        
    case DeviceType::GPU:
        return dev.index >= 0 && dev.index < config_.gpuCount;
        
    case DeviceType::NPU:
        return dev.index >= 0 && dev.index < config_.npuCount;
        
    default:
        return false;
    }
}

void DeviceRuntime::shutdown()
{
    if (shutdown_)
    {
        return;  // 幂等
    }
    
    XLOG_INFO("DeviceRuntime::shutdown: Starting...", CURRENT_THREAD_ID);
    
    shutdown_ = true;
    
    // 1. 关闭 CPU 队列
    if (cpuQueue_)
    {
        cpuQueue_->shutdown();
        XLOG_INFO("DeviceRuntime::shutdown: cpu:0 queue shutdown", CURRENT_THREAD_ID);
    }
    
    // 2. 关闭所有 GPU 队列
    for (size_t i = 0; i < gpuQueues_.size(); ++i)
    {
        if (gpuQueues_[i])
        {
            gpuQueues_[i]->shutdown();
            XLOG_INFO("DeviceRuntime::shutdown: gpu:" + std::to_string(i) + " queue shutdown", CURRENT_THREAD_ID);
        }
    }
    
    // 3. 关闭所有 NPU 队列
    for (size_t i = 0; i < npuQueues_.size(); ++i)
    {
        if (npuQueues_[i])
        {
            npuQueues_[i]->shutdown();
            XLOG_INFO("DeviceRuntime::shutdown: npu:" + std::to_string(i) + " queue shutdown", CURRENT_THREAD_ID);
        }
    }
    
    XLOG_INFO("DeviceRuntime::shutdown: Completed", CURRENT_THREAD_ID);
}

} // namespace XVisual
