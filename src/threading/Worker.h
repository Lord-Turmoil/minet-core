/**
 * @author Tony S.
 * @details Thread pool worker.
 * @ref https://github.com/inkooboo/thread-pool-cpp.git
 */

#pragma once

#include "minet/common/Base.h"

#include "threading/Queue.h"

#include <atomic>
#include <chrono>
#include <thread>

MINET_BEGIN

namespace detail
{

inline size_t MakePowerOfTwo(size_t x)
{
    if (x == 0)
    {
        return 1;
    }

    // Filling all bits after the significant one.
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x |= x >> 32;
    return x + 1;
}

} // namespace detail

namespace threading
{

/**
 * @brief Thread pool worker.
 * @note
 * Each worker has its own task queue, and thread pool will distribute
 * tasks to workers with round-robin strategy. Then, workers can "steal"
 * tasks from their neighbor, which is the donor here, to keep themselves
 * busy.
 * @note
 * Although workers run asynchronously, they are managed in one thread by
 * the pool. So, there should be no concurrent problem in this layer.
 */
template <typename TTask> class Worker final
{
public:
    Worker(size_t capacity);
    ~Worker() = default;

    Worker(const Worker&) = delete;
    Worker& operator=(const Worker&) = delete;
    Worker(Worker&& other) noexcept;
    Worker& operator=(Worker&& other) noexcept;

    void Start(Worker* donor);
    void Stop();

    /**
     * @brief Post a task to the worker.
     * @tparam T For std::forward to work
     * @param task The task.
     * @return true on success, false if the worker is at maximum capacity.
     */
    template <typename T> bool Post(T&& task);

private:
    bool _Steal(TTask* task);
    void _Routine(Worker* donor);

private:
    Queue<TTask> _queue;
    std::thread _thread;
    std::atomic<bool> _running;
};

template <typename TTask>
Worker<TTask>::Worker(size_t capacity) : _queue(detail::MakePowerOfTwo(capacity)), _running(false)
{
}

template <typename TTask> Worker<TTask>::Worker(Worker&& other) noexcept
{
    _queue = std::move(other._queue);
    _thread = std::move(other._thread);
    _running = other._running;
}

template <typename TTask> Worker<TTask>& Worker<TTask>::operator=(Worker&& other) noexcept
{
    if (this != &other)
    {
        _queue = std::move(other._queue);
        _thread = std::move(other._thread);
        _running = other._running;
    }
    return *this;
}

template <typename TTask> void Worker<TTask>::Start(Worker* donor)
{
    _running.store(true, std::memory_order_relaxed);
    _thread = std::thread(&Worker::_Routine, this, donor);
}

template <typename TTask> void Worker<TTask>::Stop()
{
    _running.store(false, std::memory_order_relaxed);
    _thread.join();
}

template <typename TTask> template <typename T> bool Worker<TTask>::Post(T&& task)
{
    return _queue.Push(std::forward<T>(task));
}

template <typename TTask> bool Worker<TTask>::_Steal(TTask* task)
{
    return _queue.Pop(task);
}

template <typename TTask> void Worker<TTask>::_Routine(Worker* donor)
{
    TTask task;
    while (_running.load(std::memory_order_relaxed))
    {
        // Get one from the worker itself, or steal one to keep busy.
        if (_queue.Pop(&task) || donor->_Steal(&task))
        {
            task();
        }
        else
        {
            // Avoid consuming too much CPU.
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

}; // namespace threading

MINET_END
