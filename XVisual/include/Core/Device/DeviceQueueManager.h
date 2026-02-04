#ifndef XVISUAL_CORE_DEVICE_DEVICEQUEUEMANAGER_H
#define XVISUAL_CORE_DEVICE_DEVICEQUEUEMANAGER_H

#include "IDeviceQueue.h"
#include "CPUQueue.h"
#include "SerialQueue.h"
#include <memory>

namespace XVisual {

/**
 * DeviceQueueManager: 设备队列管理器
 * 
 * 管理所有设备队列，提供按设备类型路由的能力。
 * 
 * PR-5.1 设计：
 * - 创建 3 种队列：CPUQueue, GPUQueue(SerialQueue), NPUQueue(SerialQueue)
 * - Any 类型由调用方映射到 CPU（不在 manager 内决策）
 * - shutdown() 按顺序关闭所有队列
 * 
 * 销毁顺序约束：
 * - GraphExecutor 析构时应先 cancel/wait，再调用 manager.shutdown()
 * - shutdown() 必须在所有任务完成或被丢弃后才返回
 */
class DeviceQueueManager {
public:
    /**
     * 构造函数
     * 
     * 创建所有设备队列。
     */
    DeviceQueueManager();
    
    /**
     * 析构函数
     * 
     * 如果未调用 shutdown()，析构时会自动调用。
     */
    ~DeviceQueueManager();
    
    // 禁止拷贝
    DeviceQueueManager(const DeviceQueueManager&) = delete;
    DeviceQueueManager& operator=(const DeviceQueueManager&) = delete;
    
    /**
     * 获取指定设备类型的队列
     * 
     * @param type 设备类型（CPU, GPU, NPU）
     * @return 对应的队列指针，永不为 nullptr
     * 
     * 注意：
     * - 不处理 Any 类型，调用方应自行映射（PR-5.1 映射到 CPU）
     * - shutdown 后调用返回的队列仍有效，但 submit 会被忽略
     */
    IDeviceQueue* getQueue(DeviceType type);
    
    /**
     * 关闭所有队列
     * 
     * 按顺序关闭：CPU -> GPU -> NPU
     * 幂等操作，多次调用安全。
     */
    void shutdown();
    
    /**
     * 是否已关闭
     */
    bool isShutdown() const { return shutdown_; }
    
private:
    std::unique_ptr<CPUQueue> cpuQueue_;
    std::unique_ptr<SerialQueue> gpuQueue_;
    std::unique_ptr<SerialQueue> npuQueue_;
    bool shutdown_ = false;
};

} // namespace XVisual

#endif // XVISUAL_CORE_DEVICE_DEVICEQUEUEMANAGER_H
