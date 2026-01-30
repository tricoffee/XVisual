#include "Core/Executor/ThreadPool.h"
#include <algorithm>

namespace XVisual {

ThreadPool::ThreadPool(size_t numThreads)
    : stopped_(false)
    , activeTasks_(0)
{
    // 默认线程数：hardware_concurrency - 1，至少 1 个
    if (numThreads == 0)
    {
        numThreads = std::max(1u, std::thread::hardware_concurrency() - 1);
    }

    workers_.reserve(numThreads);
    for (size_t i = 0; i < numThreads; ++i)
    {
        workers_.emplace_back([this] { workerLoop(); });
    }
}

ThreadPool::~ThreadPool()
{
    shutdown();
}

void ThreadPool::submit(std::function<void()> task)
{
    if (stopped_.load())
        return;
    
    tasks_.push(std::move(task));
}

void ThreadPool::shutdown()
{
    if (stopped_.exchange(true))
        return;  // 已经停止过了
    
    tasks_.stop();
    
    // std::jthread 会在析构时自动 request_stop 并 join
    workers_.clear();
}

void ThreadPool::waitAll()
{
    std::unique_lock<std::mutex> lock(waitMutex_);
    waitCv_.wait(lock, [this] {
        return tasks_.empty() && activeTasks_.load() == 0;
    });
}

void ThreadPool::workerLoop()
{
    while (!stopped_.load())
    {
        auto task = tasks_.pop();
        if (!task.has_value())
        {
            // 队列停止或为空
            break;
        }

        ++activeTasks_;
        try
        {
            (*task)();
        }
        catch (...)
        {
            // 任务内部异常，不让它传播出来
            // 实际应用中可以记录日志
        }
        --activeTasks_;

        // 通知可能在等待的 waitAll()
        waitCv_.notify_all();
    }
}

} // namespace XVisual
