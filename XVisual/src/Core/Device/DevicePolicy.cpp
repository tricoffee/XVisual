#include "Core/Device/DevicePolicy.h"
#include "Core/Device/DeviceRuntime.h"
#include "Core/Executor/INode.h"
#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"

namespace XVisual {

// ============================================================================
// FixedPolicy
// ============================================================================

DeviceInstance FixedPolicy::pick(const INode& node, const DeviceRuntime& runtime)
{
    DeviceType preferred = node.preferredDevice();
    DeviceInstance result;
    
    switch (preferred)
    {
    case DeviceType::CPU:
        result = DeviceInstance::CPU(0);
        break;
        
    case DeviceType::GPU:
        if (runtime.getGPUCount() > 0)
        {
            result = DeviceInstance::GPU(0);
        }
        else
        {
            // 没有 GPU，fallback 到 CPU
            XLOG_INFO("FixedPolicy: No GPU available, fallback to cpu:0", CURRENT_THREAD_ID);
            result = DeviceInstance::CPU(0);
        }
        break;
        
    case DeviceType::NPU:
        if (runtime.getNPUCount() > 0)
        {
            result = DeviceInstance::NPU(0);
        }
        else
        {
            // 没有 NPU，fallback 到 CPU
            XLOG_INFO("FixedPolicy: No NPU available, fallback to cpu:0", CURRENT_THREAD_ID);
            result = DeviceInstance::CPU(0);
        }
        break;
        
    case DeviceType::Any:
    default:
        // Any → CPU（保持 PR-5.1 兼容）
        result = DeviceInstance::CPU(0);
        break;
    }
    
    XLOG_INFO("FixedPolicy: " + deviceTypeToString(preferred) + " -> " + result.name(), CURRENT_THREAD_ID);
    return result;
}

// ============================================================================
// RoundRobinPolicy
// ============================================================================

DeviceInstance RoundRobinPolicy::pick(const INode& node, const DeviceRuntime& runtime)
{
    DeviceType preferred = node.preferredDevice();
    DeviceInstance result;
    
    switch (preferred)
    {
    case DeviceType::CPU:
        result = DeviceInstance::CPU(0);
        break;
        
    case DeviceType::GPU:
        {
            int gpuCount = runtime.getGPUCount();
            if (gpuCount > 0)
            {
                // 轮询选择 GPU
                int index = gpuRoundRobin_.fetch_add(1) % gpuCount;
                result = DeviceInstance::GPU(index);
            }
            else
            {
                XLOG_INFO("RoundRobinPolicy: No GPU available, fallback to cpu:0", CURRENT_THREAD_ID);
                result = DeviceInstance::CPU(0);
            }
        }
        break;
        
    case DeviceType::NPU:
        {
            int npuCount = runtime.getNPUCount();
            if (npuCount > 0)
            {
                // 轮询选择 NPU
                int index = npuRoundRobin_.fetch_add(1) % npuCount;
                result = DeviceInstance::NPU(index);
            }
            else
            {
                XLOG_INFO("RoundRobinPolicy: No NPU available, fallback to cpu:0", CURRENT_THREAD_ID);
                result = DeviceInstance::CPU(0);
            }
        }
        break;
        
    case DeviceType::Any:
    default:
        // Any → CPU（保持 PR-5.1 兼容）
        result = DeviceInstance::CPU(0);
        break;
    }
    
    XLOG_INFO("RoundRobinPolicy: " + deviceTypeToString(preferred) + " -> " + result.name(), CURRENT_THREAD_ID);
    return result;
}

// ============================================================================
// GpuPreferPolicy
// ============================================================================

DeviceInstance GpuPreferPolicy::pick(const INode& node, const DeviceRuntime& runtime)
{
    DeviceType preferred = node.preferredDevice();
    DeviceInstance result;
    
    switch (preferred)
    {
    case DeviceType::CPU:
        result = DeviceInstance::CPU(0);
        break;
        
    case DeviceType::GPU:
        if (runtime.getGPUCount() > 0)
        {
            result = DeviceInstance::GPU(0);
        }
        else
        {
            XLOG_INFO("GpuPreferPolicy: No GPU available, fallback to cpu:0", CURRENT_THREAD_ID);
            result = DeviceInstance::CPU(0);
        }
        break;
        
    case DeviceType::NPU:
        if (runtime.getNPUCount() > 0)
        {
            result = DeviceInstance::NPU(0);
        }
        else
        {
            XLOG_INFO("GpuPreferPolicy: No NPU available, fallback to cpu:0", CURRENT_THREAD_ID);
            result = DeviceInstance::CPU(0);
        }
        break;
        
    case DeviceType::Any:
        // Any → GPU 优先（如果有），否则 CPU
        if (runtime.getGPUCount() > 0)
        {
            result = DeviceInstance::GPU(0);
            XLOG_INFO("GpuPreferPolicy: Any -> gpu:0 (GPU preferred)", CURRENT_THREAD_ID);
        }
        else
        {
            result = DeviceInstance::CPU(0);
            XLOG_INFO("GpuPreferPolicy: Any -> cpu:0 (no GPU available)", CURRENT_THREAD_ID);
        }
        break;
        
    default:
        result = DeviceInstance::CPU(0);
        break;
    }
    
    XLOG_INFO("GpuPreferPolicy: " + deviceTypeToString(preferred) + " -> " + result.name(), CURRENT_THREAD_ID);
    return result;
}

} // namespace XVisual
