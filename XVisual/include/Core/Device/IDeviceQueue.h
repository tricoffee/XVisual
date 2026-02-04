#ifndef XVISUAL_CORE_DEVICE_IDEVICEQUEUE_H
#define XVISUAL_CORE_DEVICE_IDEVICEQUEUE_H

#include <functional>
#include <string>
#include "DeviceType.h"

namespace XVisual {

/**
 * IDeviceQueue: 设备计算队列抽象接口
 * 
 * 不同设备类型有不同的并发模型：
 * - CPU: 线程池（多 worker 并行）
 * - GPU: 串行队列（模拟单 CUDA stream）
 * - NPU: 串行队列（模拟单 NPU context）
 * 
 * 设计约束（PR-5.1）：
 * - 不返回 future，完成性由调用方通过 callback 或外部计数管理
 * - 队列层只负责排队/执行，不理解 DAG 取消语义
 * - shutdown() 必须能让 worker 线程安全退出
 */
class IDeviceQueue {
public:
    virtual ~IDeviceQueue() = default;
    
    /**
     * 提交任务到队列
     * 
     * @param task 任务函数，执行完成后应由调用方处理完成逻辑
     * 
     * 注意：
     * - 不返回 future，完成性由外部 nodeStates/remaining_ 管理
     * - shutdown 后调用 submit 的行为由实现定义（建议忽略或记录日志）
     */
    virtual void submit(std::function<void()> task) = 0;
    
    /**
     * 获取队列关联的设备类型
     */
    virtual DeviceType deviceType() const = 0;
    
    /**
     * 获取队列名称（用于日志）
     */
    virtual std::string name() const = 0;
    
    /**
     * 关闭队列
     * 
     * 语义：
     * - 停止接收新任务
     * - 唤醒 worker 线程使其能够退出
     * - 等待 worker 线程结束（join）
     * - 已入队但未执行的任务将被丢弃
     * 
     * 调用后 submit() 的行为由实现定义。
     */
    virtual void shutdown() = 0;
};

} // namespace XVisual

#endif // XVISUAL_CORE_DEVICE_IDEVICEQUEUE_H
