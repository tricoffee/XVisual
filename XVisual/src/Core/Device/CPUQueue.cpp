#include "Core/Device/CPUQueue.h"
#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"

namespace XVisual {

CPUQueue::CPUQueue(size_t numWorkers)
    : shutdown_(false)
{
    // 确定 worker 数量
    if (numWorkers == 0) {
        numWorkers = std::thread::hardware_concurrency();
        if (numWorkers == 0) {
            numWorkers = 4;  // 默认值
        }
    }
    
    // 启动 worker 线程
    workers_.reserve(numWorkers);
    for (size_t i = 0; i < numWorkers; ++i) {
        workers_.emplace_back(&CPUQueue::workerLoop, this);
    }
    
    XLOG_INFO("CPUQueue created with " + std::to_string(numWorkers) + " workers", 
              CURRENT_THREAD_ID);
}

CPUQueue::~CPUQueue()
{
    // 确保 shutdown 被调用
    if (!shutdown_.load()) {
        shutdown();
    }
}

void CPUQueue::submit(std::function<void()> task)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (shutdown_.load()) {
            XLOG_INFO("CPUQueue::submit: Ignored after shutdown", CURRENT_THREAD_ID);
            return;
        }
        
        tasks_.push(std::move(task));
    }
    
    // 通知一个等待的 worker
    cv_.notify_one();
}

void CPUQueue::shutdown()
{
    // 标记 shutdown（原子操作）
    bool expected = false;
    if (!shutdown_.compare_exchange_strong(expected, true)) {
        // 已经 shutdown 过了
        return;
    }
    
    XLOG_INFO("CPUQueue::shutdown: Starting...", CURRENT_THREAD_ID);
    
    // 唤醒所有 worker 使其能够退出
    cv_.notify_all();
    
    // 等待所有 worker 线程结束
    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    
    // 清空未执行的任务
    size_t droppedCount = 0;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        droppedCount = tasks_.size();
        while (!tasks_.empty()) {
            tasks_.pop();
        }
    }
    
    if (droppedCount > 0) {
        XLOG_INFO("CPUQueue::shutdown: Dropped " + std::to_string(droppedCount) + 
                  " pending tasks", CURRENT_THREAD_ID);
    }
    
    XLOG_INFO("CPUQueue::shutdown: Completed", CURRENT_THREAD_ID);
}

void CPUQueue::workerLoop()
{
    XLOG_INFO("CPUQueue worker started", CURRENT_THREAD_ID);
    
    while (true) {
        std::function<void()> task;
        
        {
            std::unique_lock<std::mutex> lock(mutex_);
            
            // 等待条件：有任务 或 shutdown
            cv_.wait(lock, [this]() {
                return !tasks_.empty() || shutdown_.load();
            });
            
            // shutdown 且队列为空时退出
            if (shutdown_.load() && tasks_.empty()) {
                break;
            }
            
            // 取出一个任务
            if (!tasks_.empty()) {
                task = std::move(tasks_.front());
                tasks_.pop();
            }
        }
        
        // 执行任务（在锁外，允许并行执行）
        if (task) {
            task();
        }
    }
    
    XLOG_INFO("CPUQueue worker exiting", CURRENT_THREAD_ID);
}

} // namespace XVisual
