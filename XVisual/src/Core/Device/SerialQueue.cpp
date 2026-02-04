#include "Core/Device/SerialQueue.h"
#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"

namespace XVisual {

SerialQueue::SerialQueue(DeviceType type, const std::string& queueName)
    : type_(type)
    , name_(queueName)
    , shutdown_(false)
{
    // 启动 worker 线程
    worker_ = std::thread(&SerialQueue::workerLoop, this);
    XLOG_INFO(name_ + " created", CURRENT_THREAD_ID);
}

SerialQueue::~SerialQueue()
{
    // 确保 shutdown 被调用
    if (!shutdown_.load()) {
        shutdown();
    }
}

void SerialQueue::submit(std::function<void()> task)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (shutdown_.load()) {
            XLOG_INFO(name_ + "::submit: Ignored after shutdown", CURRENT_THREAD_ID);
            return;
        }
        
        tasks_.push(std::move(task));
    }
    
    // 通知 worker 有新任务
    cv_.notify_one();
}

void SerialQueue::shutdown()
{
    // 标记 shutdown（原子操作）
    bool expected = false;
    if (!shutdown_.compare_exchange_strong(expected, true)) {
        // 已经 shutdown 过了
        return;
    }
    
    XLOG_INFO(name_ + "::shutdown: Starting...", CURRENT_THREAD_ID);
    
    // 唤醒 worker 使其能够退出
    cv_.notify_all();
    
    // 等待 worker 线程结束
    if (worker_.joinable()) {
        worker_.join();
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
        XLOG_INFO(name_ + "::shutdown: Dropped " + std::to_string(droppedCount) + 
                  " pending tasks", CURRENT_THREAD_ID);
    }
    
    XLOG_INFO(name_ + "::shutdown: Completed", CURRENT_THREAD_ID);
}

void SerialQueue::workerLoop()
{
    XLOG_INFO(name_ + " worker started", CURRENT_THREAD_ID);
    
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
        
        // 执行任务（在锁外，允许串行执行）
        if (task) {
            task();
        }
    }
    
    XLOG_INFO(name_ + " worker exiting", CURRENT_THREAD_ID);
}

} // namespace XVisual
