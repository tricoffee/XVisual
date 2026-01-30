#ifndef XVISUAL_CORE_EXECUTOR_THREADSAFEQUEUE_H
#define XVISUAL_CORE_EXECUTOR_THREADSAFEQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <atomic>

namespace XVisual {

/**
 * ThreadSafeQueue: 线程安全的阻塞队列
 * 
 * 支持多生产者多消费者模式，用于任务调度。
 */
template <typename T>
class ThreadSafeQueue
{
public:
    ThreadSafeQueue() : stopped_(false) {}

    ~ThreadSafeQueue()
    {
        stop();
    }

    /**
     * 向队列尾部添加元素
     * 如果队列已停止，则忽略
     */
    void push(T item)
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (stopped_)
                return;
            queue_.push(std::move(item));
        }
        cv_.notify_one();
    }

    /**
     * 阻塞等待并取出队首元素
     * 如果队列停止且为空，返回 std::nullopt
     */
    std::optional<T> pop()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return stopped_ || !queue_.empty(); });
        
        if (queue_.empty())
            return std::nullopt;
        
        T item = std::move(queue_.front());
        queue_.pop();
        return item;
    }

    /**
     * 非阻塞尝试取出队首元素
     * 如果队列为空，返回 std::nullopt
     */
    std::optional<T> tryPop()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty())
            return std::nullopt;
        
        T item = std::move(queue_.front());
        queue_.pop();
        return item;
    }

    /**
     * 停止队列，唤醒所有等待的消费者
     */
    void stop()
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stopped_ = true;
        }
        cv_.notify_all();
    }

    /**
     * 重置队列状态（清空队列并取消停止标记）
     */
    void reset()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        stopped_ = false;
        std::queue<T> empty;
        std::swap(queue_, empty);
    }

    /**
     * 检查队列是否已停止
     */
    bool isStopped() const
    {
        return stopped_;
    }

    /**
     * 获取当前队列大小（近似值，仅供参考）
     */
    size_t size() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

    /**
     * 检查队列是否为空（近似值，仅供参考）
     */
    bool empty() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

private:
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<bool> stopped_;
};

} // namespace XVisual

#endif // XVISUAL_CORE_EXECUTOR_THREADSAFEQUEUE_H
