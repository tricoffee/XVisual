#ifndef XVISUAL_CORE_DEVICE_CPUQUEUE_H
#define XVISUAL_CORE_DEVICE_CPUQUEUE_H

#include "IDeviceQueue.h"
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>

namespace XVisual {

/**
 * CPUQueue: 多 worker 线程池队列
 * 
 * 支持并行执行多个任务，适用于 CPU 计算密集型节点。
 * 
 * 线程模型：
 * - 内部维护 N 个 worker 线程（默认 = hardware_concurrency）
 * - submit() 将任务入队，任意空闲 worker 执行
 * - 多任务可并行执行
 * - shutdown() 停止所有 worker
 */
class CPUQueue : public IDeviceQueue {
public:
    /**
     * 构造函数
     * 
     * @param numWorkers 工作线程数，0 表示使用 hardware_concurrency
     */
    explicit CPUQueue(size_t numWorkers = 0);
    
    /**
     * 析构函数
     * 
     * 如果未调用 shutdown()，析构时会自动调用。
     */
    ~CPUQueue() override;
    
    /**
     * 提交任务到队列
     * 
     * 线程安全。shutdown 后调用将被忽略。
     */
    void submit(std::function<void()> task) override;
    
    DeviceType deviceType() const override { return DeviceType::CPU; }
    std::string name() const override { return "CPUQueue"; }
    
    /**
     * 关闭队列
     * 
     * - 标记 shutdown
     * - 唤醒所有 worker
     * - join 所有 worker 线程
     * - 清空未执行的任务
     */
    void shutdown() override;
    
    /**
     * 获取 worker 数量
     */
    size_t workerCount() const { return workers_.size(); }
    
private:
    /**
     * worker 线程主循环
     */
    void workerLoop();
    
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<bool> shutdown_{false};
};

} // namespace XVisual

#endif // XVISUAL_CORE_DEVICE_CPUQUEUE_H
