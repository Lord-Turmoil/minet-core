/**
 * @author Tony S.
 * @details The thread pool.
 */

#pragma once

#include "minet/common/Base.h"

#include "threading/Worker.h"

#include <atomic>
#include <functional>
#include <future>
#include <utility>

MINET_BEGIN

namespace threading
{

unsigned int HardwareConcurrency();

/**
 * @brief The thread pool.
 * @note
 * The thread pool manages collection of workers using the following strategy:
 *   1. Round-robin: Distribute tasks to workers in a round-robin fashion.
 *   2. Stealing: Workers can steal tasks from their neighbor to keep themselves busy.
 */
class ThreadPool final
{
public:
    using TaskFn = std::function<void()>;

    /**
     * @brief Create a thread pool. Won't start the workers.
     * @param threads Number of threads in the pool.
     * @param capacity Capacity of each worker, should be in power of 2.
     * @note The maximum capacity of the thread pool is thread * capacity.
     * @note
     * If capacity is not power of 2, it will be adjusted to the nearest
     * power of 2 that is greater than or equal to the given capacity.
     */
    explicit ThreadPool(unsigned int threads, size_t capacity = 1024u);
    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&& other) noexcept;
    ThreadPool& operator=(ThreadPool&& other) noexcept;

    /**
     * @brief Submit a task to the thread pool. Used the naming in Java.
     * @tparam T In order to use std::forward.
     * @param task The task to submit.
     * @return true on submitted, false if failed.
     * @note
     * Return false doesn't mean the thread pool is completely full. It is
     * just that the current selected worker is full.
     */
    template <typename T> bool Submit(T&& task);

private:
    unsigned int _threads;
    bool _running;

    /**
     * @brief Here, we use pointer to avoid move overhead.
     */
    std::vector<Ref<Worker<TaskFn>>> _workers;
    std::atomic<size_t> _next;
};

template <typename T> bool ThreadPool::Submit(T&& task)
{
    size_t next = _next.fetch_add(1, std::memory_order_relaxed) % _threads;
    return _workers[next]->Post(std::forward<T>(task));
}

} // namespace threading

MINET_END
