#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include "Core/Device/DeviceSpec.h"
#include <vector>
#include <mutex>

namespace XVisual {

/**
 * DeviceInfo: 设备详细信息
 */
struct DeviceInfo
{
    DeviceSpec spec;
    size_t totalMemory = 0;      // 总内存 (bytes)
    size_t freeMemory = 0;       // 可用内存 (bytes)
    int computeCapability = 0;   // 算力等级 (可选, 如 CUDA compute capability)
    bool isAvailable = true;     // 是否可用
    
    DeviceInfo() = default;
    DeviceInfo(const DeviceSpec& s) : spec(s) {}
};

/**
 * DeviceManager: 设备发现与查询
 * 
 * 单例模式，负责：
 * 1. 发现系统中可用的 CPU/GPU/NPU 设备
 * 2. 提供设备信息查询接口
 * 3. 不依赖任何推理框架
 * 
 * 注意：PR-5.2 阶段仅实现基础的设备发现（CPU 固定1个，GPU 通过环境检测）。
 * 后续 PR 可增强为真实的 CUDA/OpenCL 设备枚举。
 */
class DeviceManager
{
public:
    static DeviceManager& instance();
    
    /**
     * 设备发现 (启动时调用一次)
     * 
     * 扫描系统中的 CPU/GPU/NPU 设备并缓存信息。
     */
    void discoverDevices();
    
    /**
     * 获取所有设备
     */
    std::vector<DeviceInfo> getAllDevices() const;
    
    /**
     * 获取指定类型的设备
     */
    std::vector<DeviceInfo> getDevices(DeviceType type) const;
    
    /**
     * 检查是否有指定类型的设备
     */
    bool hasDevice(DeviceType type) const;
    
    /**
     * 获取指定类型设备的数量
     */
    int getDeviceCount(DeviceType type) const;
    
    /**
     * 获取默认设备
     * 
     * 如果请求 GPU 但没有可用 GPU，返回 CPU。
     */
    DeviceSpec getDefaultDevice(DeviceType type) const;
    
    /**
     * 根据 DeviceType::Any 解析为实际设备
     * 
     * 优先级: GPU > NPU > CPU
     */
    DeviceSpec resolveAny() const;

private:
    DeviceManager() = default;
    ~DeviceManager() = default;
    DeviceManager(const DeviceManager&) = delete;
    DeviceManager& operator=(const DeviceManager&) = delete;
    
    void discoverCPU();
    void discoverGPU();
    void discoverNPU();
    
    std::vector<DeviceInfo> devices_;
    mutable std::mutex mutex_;
    bool discovered_ = false;
};

} // namespace XVisual

#endif // DEVICEMANAGER_H
