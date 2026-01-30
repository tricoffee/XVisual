#ifndef XVISUAL_CORE_EXECUTOR_THREADPOOL_H
#define XVISUAL_CORE_EXECUTOR_THREADPOOL_H

#include <vector>
#include <thread>
#include <functional>
#include <atomic>
#include <memory>
#include "ThreadSafeQueue.h"

namespace XVisual {

/**
 * ThreadPool: 简单的线程池实现
 * 
 * 使用 C++20 std::jthread 实现优雅停止。
 * 支持任务提交和等待所有任务完成。
 */
class ThreadPool
{
public:
    /**
     * 构造函数
     * @param numThreads 工作线程数，默认为 hardware_concurrency - 1
     */
    explicit ThreadPool(size_t numThreads = 0);

    /**
     * 析构函数，自动停止所有工作线程
     */
    ~ThreadPool();

    // 禁止拷贝
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    /**
     * 提交任务到线程池
     * @param task 要执行的任务
     */
    void submit(std::function<void()> task);

    /**
     * 关闭线程池，停止接受新任务
     * 已提交的任务会继续执行完成
     */
    void shutdown();

    /**
     * 等待所有任务完成
     */
    void waitAll();

    /**
     * 获取工作线程数
     */
    size_t size() const { return workers_.size(); }

    /**
     * 检查线程池是否正在运行
     */
    bool isRunning() const { return !stopped_.load(); }

    /**
     * 获取待处理任务数（近似值）
     */
    size_t pendingTasks() const { return tasks_.size(); }

private:
    void workerLoop();

    std::vector<std::jthread> workers_;
    ThreadSafeQueue<std::function<void()>> tasks_;
    std::atomic<bool> stopped_;
    std::atomic<int> activeTasks_;  // 正在执行的任务数
    std::mutex waitMutex_;
    std::condition_variable waitCv_;
};

} // namespace XVisual

#endif // XVISUAL_CORE_EXECUTOR_THREADPOOL_H
