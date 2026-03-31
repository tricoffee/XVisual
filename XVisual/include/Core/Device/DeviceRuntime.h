#ifndef XVISUAL_CORE_DEVICE_DEVICERUNTIME_H
#define XVISUAL_CORE_DEVICE_DEVICERUNTIME_H

#include "Core/Device/DeviceInstance.h"
#include "Core/Device/IDeviceQueue.h"
#include "Core/Device/SerialQueue.h"
#include "Core/Device/CPUQueue.h"
#include <memory>
#include <vector>
#include <mutex>

namespace XVisual {

/**
 * DeviceRuntimeConfig: 设备运行时配置
 * 
 * PR-5.3: 静态配置设备数量（不做真实枚举）
 */
struct DeviceRuntimeConfig
{
    int gpuCount = 1;       // GPU 实例数量（默认 1 个占位）
    int npuCount = 0;       // NPU 实例数量（默认 0 个）
    size_t cpuThreads = 0;  // CPU 线程池大小（0=自动）
    
    DeviceRuntimeConfig() = default;
    DeviceRuntimeConfig(int gpu, int npu = 0, size_t cpu = 0)
        : gpuCount(gpu), npuCount(npu), cpuThreads(cpu) {}
};

/**
 * DeviceRuntime: 设备运行时管理器
 * 
 * 职责：
 * - 枚举设备实例（PR-5.3 使用静态配置）
 * - 为每个实例持有一个队列：
 *   - CPU: 共享一个 CPUQueue（线程池）
 *   - GPU: 每实例一个 SerialQueue
 *   - NPU: 每实例一个 SerialQueue
 * - 提供队列获取接口
 * 
 * 与 DeviceQueueManager（PR-5.1）的区别：
 * - DeviceQueueManager: 固定 3 个队列（CPU/GPU/NPU 各一个）
 * - DeviceRuntime: 支持多 GPU/NPU 实例，可配置数量
 * 
 * 与 DeviceManager 的区别：
 * - DeviceManager: 设备发现与信息查询（静态）
 * - DeviceRuntime: 设备队列管理与任务调度（运行时）
 */
class DeviceRuntime
{
public:
    /**
     * 构造函数
     * 
     * @param config 设备配置（设备数量等）
     */
    explicit DeviceRuntime(const DeviceRuntimeConfig& config = {});
    
    /**
     * 析构函数
     * 
     * 如果未调用 shutdown()，析构时自动调用。
     */
    ~DeviceRuntime();
    
    // 禁止拷贝
    DeviceRuntime(const DeviceRuntime&) = delete;
    DeviceRuntime& operator=(const DeviceRuntime&) = delete;
    
    /**
     * 列出指定类型的所有设备实例
     * 
     * @param type 设备类型（CPU/GPU/NPU）
     * @return 该类型的所有实例
     * 
     * 注意：
     * - CPU 返回 [cpu:0]（单实例）
     * - GPU 返回 [gpu:0, gpu:1, ...]（根据配置）
     * - NPU 返回 [npu:0, npu:1, ...]（根据配置）
     * - Any 返回空（不是具体类型）
     */
    std::vector<DeviceInstance> list(DeviceType type) const;
    
    /**
     * 获取指定设备实例的队列
     * 
     * @param dev 设备实例
     * @return 队列引用（永不失效）
     * @throws std::out_of_range 如果实例不存在
     */
    IDeviceQueue& queue(const DeviceInstance& dev);
    
    /**
     * 检查设备实例是否存在
     */
    bool hasDevice(const DeviceInstance& dev) const;
    
    /**
     * 获取配置
     */
    const DeviceRuntimeConfig& config() const { return config_; }
    
    /**
     * 获取 GPU 数量
     */
    int getGPUCount() const { return config_.gpuCount; }
    
    /**
     * 获取 NPU 数量
     */
    int getNPUCount() const { return config_.npuCount; }
    
    /**
     * 关闭所有队列
     * 
     * 按顺序关闭：CPU -> GPU[0..N] -> NPU[0..M]
     * 幂等操作，多次调用安全。
     */
    void shutdown();
    
    /**
     * 是否已关闭
     */
    bool isShutdown() const { return shutdown_; }

private:
    DeviceRuntimeConfig config_;
    
    // CPU 队列（共享一个线程池，只有 cpu:0）
    std::unique_ptr<CPUQueue> cpuQueue_;
    
    // GPU 队列（每实例一个 SerialQueue）
    std::vector<std::unique_ptr<SerialQueue>> gpuQueues_;
    
    // NPU 队列（每实例一个 SerialQueue）
    std::vector<std::unique_ptr<SerialQueue>> npuQueues_;
    
    bool shutdown_ = false;
    mutable std::mutex mutex_;
};

} // namespace XVisual

#endif // XVISUAL_CORE_DEVICE_DEVICERUNTIME_H
