#ifndef XVISUAL_CORE_DEVICE_SERIALQUEUE_H
#define XVISUAL_CORE_DEVICE_SERIALQUEUE_H

#include "IDeviceQueue.h"
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace XVisual {

/**
 * SerialQueue: 单 worker 串行队列
 * 
 * 用于 GPU/NPU 设备，保证同一时间只有一个任务在执行。
 * 这模拟了 GPU 的单 CUDA stream 或 NPU 的单 context 独占语义。
 * 
 * PR-5.1：任务实际仍在 CPU 上执行（占位），但保证串行。
 * 
 * 线程模型：
 * - 内部维护一个 worker 线程
 * - submit() 将任务入队并通知 worker
 * - worker 循环从队列取任务执行（串行，不并发）
 * - shutdown() 标记停止、唤醒 worker、join 线程
 */
class SerialQueue : public IDeviceQueue {
public:
    /**
     * 构造函数
     * 
     * @param type 设备类型（GPU 或 NPU）
     * @param queueName 队列名称（用于日志）
     */
    SerialQueue(DeviceType type, const std::string& queueName);
    
    /**
     * 析构函数
     * 
     * 如果未调用 shutdown()，析构时会自动调用。
     */
    ~SerialQueue() override;
    
    /**
     * 提交任务到队列
     * 
     * 线程安全。shutdown 后调用将被忽略（记录日志）。
     */
    void submit(std::function<void()> task) override;
    
    DeviceType deviceType() const override { return type_; }
    std::string name() const override { return name_; }
    
    /**
     * 关闭队列
     * 
     * - 标记 shutdown
     * - 唤醒 worker（notify_all）
     * - join worker 线程
     * - 清空未执行的任务（记录丢弃数量）
     */
    void shutdown() override;
    
private:
    /**
     * worker 线程主循环
     */
    void workerLoop();
    
    DeviceType type_;
    std::string name_;
    
    std::thread worker_;
    std::queue<std::function<void()>> tasks_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<bool> shutdown_{false};
};

} // namespace XVisual

#endif // XVISUAL_CORE_DEVICE_SERIALQUEUE_H
