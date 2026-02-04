#include "Core/Device/DeviceManager.h"
#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include <algorithm>

// Windows-specific headers for GPU detection
#ifdef _WIN32
#include <windows.h>
#endif

namespace XVisual {

DeviceManager& DeviceManager::instance()
{
    static DeviceManager mgr;
    return mgr;
}

void DeviceManager::discoverDevices()
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (discovered_)
    {
        return;  // 已经发现过，不重复执行
    }
    
    devices_.clear();
    
    discoverCPU();
    discoverGPU();
    discoverNPU();
    
    discovered_ = true;
    
    XLOG_INFO("DeviceManager: Discovered " + std::to_string(getDeviceCount(DeviceType::CPU)) + " CPU, " +
              std::to_string(getDeviceCount(DeviceType::GPU)) + " GPU, " +
              std::to_string(getDeviceCount(DeviceType::NPU)) + " NPU devices", CURRENT_THREAD_ID);
}

void DeviceManager::discoverCPU()
{
    // CPU 固定1个
    DeviceInfo cpuInfo;
    cpuInfo.spec = DeviceSpec::CPU();
    cpuInfo.isAvailable = true;
    cpuInfo.totalMemory = 0;  // 可以后续通过系统 API 获取
    cpuInfo.freeMemory = 0;
    
    devices_.push_back(cpuInfo);
    XLOG_INFO("DeviceManager: Found CPU device", CURRENT_THREAD_ID);
}

void DeviceManager::discoverGPU()
{
    // PR-5.2 简化实现：检查环境变量 CUDA_VISIBLE_DEVICES 或假设有1个 GPU
    // 后续 PR 可以使用 CUDA API 或 TensorFlow 的设备列表来真实枚举
    
    // 简单检测：假设如果 TensorFlow GPU 版本可用，则有 GPU
    // 这里先假设有1个 GPU（占位实现）
    // 实际部署时可以通过 TF_GetGpuCount 或 CUDA API 检测
    
#ifdef _WIN32
    // 检查是否有 NVIDIA GPU (通过检测 nvcuda.dll)
    HMODULE hNvCuda = LoadLibraryA("nvcuda.dll");
    if (hNvCuda != nullptr)
    {
        FreeLibrary(hNvCuda);
        
        // 假设有1个 GPU
        DeviceInfo gpuInfo;
        gpuInfo.spec = DeviceSpec::GPU(0);
        gpuInfo.isAvailable = true;
        gpuInfo.totalMemory = 0;  // 可以通过 CUDA API 获取
        gpuInfo.freeMemory = 0;
        
        devices_.push_back(gpuInfo);
        XLOG_INFO("DeviceManager: Found GPU device (NVIDIA CUDA detected)", CURRENT_THREAD_ID);
    }
    else
    {
        XLOG_INFO("DeviceManager: No GPU device found", CURRENT_THREAD_ID);
    }
#else
    // Linux/其他平台：检查 /dev/nvidia* 或 libcuda.so
    // 暂时假设没有 GPU
    XLOG_INFO("DeviceManager: GPU detection not implemented for this platform", CURRENT_THREAD_ID);
#endif
}

void DeviceManager::discoverNPU()
{
    // PR-5.2 不实现 NPU 发现，仅占位
    // 后续可以添加 Intel OpenVINO、华为昇腾等 NPU 检测
    XLOG_INFO("DeviceManager: NPU discovery not implemented (placeholder)", CURRENT_THREAD_ID);
}

std::vector<DeviceInfo> DeviceManager::getAllDevices() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return devices_;
}

std::vector<DeviceInfo> DeviceManager::getDevices(DeviceType type) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<DeviceInfo> result;
    for (const auto& dev : devices_)
    {
        if (dev.spec.type == type)
        {
            result.push_back(dev);
        }
    }
    return result;
}

bool DeviceManager::hasDevice(DeviceType type) const
{
    return getDeviceCount(type) > 0;
}

int DeviceManager::getDeviceCount(DeviceType type) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    int count = 0;
    for (const auto& dev : devices_)
    {
        if (dev.spec.type == type)
        {
            ++count;
        }
    }
    return count;
}

DeviceSpec DeviceManager::getDefaultDevice(DeviceType type) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& dev : devices_)
    {
        if (dev.spec.type == type && dev.isAvailable)
        {
            return dev.spec;
        }
    }
    
    // 如果请求的类型不可用，fallback 到 CPU
    if (type != DeviceType::CPU)
    {
        for (const auto& dev : devices_)
        {
            if (dev.spec.type == DeviceType::CPU && dev.isAvailable)
            {
                return dev.spec;
            }
        }
    }
    
    // 最后兜底返回 CPU
    return DeviceSpec::CPU();
}

DeviceSpec DeviceManager::resolveAny() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    // 优先级: GPU > NPU > CPU
    for (const auto& dev : devices_)
    {
        if (dev.spec.type == DeviceType::GPU && dev.isAvailable)
        {
            return dev.spec;
        }
    }
    
    for (const auto& dev : devices_)
    {
        if (dev.spec.type == DeviceType::NPU && dev.isAvailable)
        {
            return dev.spec;
        }
    }
    
    // 默认 CPU
    return DeviceSpec::CPU();
}

} // namespace XVisual
