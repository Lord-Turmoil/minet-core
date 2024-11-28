/**
 * @author Tony S.
 * @details Concurrent queue to support thread pool scheduling.
 */

#pragma once

#include "minet/common/Base.h"

#include "minet/common/Assert.h"

#include <atomic>
#include <new> // std::hardware_destructive_interference_size
#include <utility>
#include <vector>

MINET_BEGIN

namespace threading
{

// It is better to align critical data to avoid false sharing.
#if defined(__cpp_lib_hardware_interference_size)
static constexpr size_t HARDWARE_INTERFERENCE_SIZE = std::hardware_destructive_interference_size;
#else
static constexpr size_t HARDWARE_INTERFERENCE_SIZE = 64;
#endif

/**
 * @brief A lock-free concurrent queue.
 * @tparam TData Type of the data in this queue.
 * @note TData must have default constructor, and is movable.
 * @warning This queue will fail if size_t overflows!
 * @ref https://www.1024cores.net/home/lock-free-algorithms/queues/bounded-mpmc-queue
 * @ref https://github.com/rigtorp/MPMCQueue
 */
template <typename TData> class Queue final
{
public:
    static_assert(std::is_move_constructible_v<TData>, "TData must be movable");

    explicit Queue(size_t capacity);
    ~Queue() = default;

    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;

    Queue(Queue&& other) noexcept;
    Queue& operator=(Queue&& other) noexcept;

    /**
     * @brief Push data into the queue.
     * @tparam T Inorder to use std::forward.
     * @param data The data to add to the queue.
     * @return true on success, false if the queue is full.
     */
    template <typename T> bool Push(T&& data);

    /**
     * @brief Pop data from the queue.
     * @param data Output the popped data.
     * @return true on success, false if the queue is empty.
     */
    bool Pop(TData* data);

private:
    struct Slot
    {
        /**
         * @brief The order of enqueue.
         * @note Node is reused, so we use index to record the order.
         * @note The index is also the order of dequeue.
         */
        alignas(HARDWARE_INTERFERENCE_SIZE) std::atomic<size_t> Index;

        /**
         * @brief The data held in this node.
         */
        TData Data;
    };

private:
    size_t _capacity; // required to be power of 2
    size_t _mask;     // _capacity - 1, so that we can use & instead of %
    std::vector<Slot> _slots;

    // Next position to enqueue.
    alignas(HARDWARE_INTERFERENCE_SIZE) std::atomic<size_t> _nextEnqueue;

    // Next position to dequeue.
    alignas(HARDWARE_INTERFERENCE_SIZE) std::atomic<size_t> _nextDequeue;
};

/*
 * ===================================================================
 * ------------------------ Implementation ---------------------------
 * ===================================================================
 */

template <typename TData>
Queue<TData>::Queue(size_t capacity)
    : _capacity(capacity), _mask(capacity - 1), _slots(capacity), _nextEnqueue(0), _nextDequeue(0)
{
    MINET_ASSERT(capacity > 0 && (capacity & (capacity - 1)) == 0, "Capacity must be power of 2");

    for (size_t i = 0; i < capacity; ++i)
    {
        _slots[i].Index.store(i, std::memory_order_relaxed);
    }
}

template <typename TData> Queue<TData>::Queue(Queue&& other) noexcept
{
    _capacity = other._capacity;
    _mask = other._mask;

    _nextEnqueue.store(other._nextEnqueue.load(std::memory_order_relaxed), std::memory_order_relaxed);
    _nextDequeue.store(other._nextDequeue.load(std::memory_order_relaxed), std::memory_order_relaxed);

    _slots = std::move(other._slots);
}

template <typename TData> Queue<TData>& Queue<TData>::operator=(Queue&& other) noexcept
{
    if (this != &other)
    {
        _capacity = other._capacity;
        _mask = other._mask;
        _nextEnqueue.store(other._nextEnqueue.load(std::memory_order_relaxed), std::memory_order_relaxed);
        _nextDequeue.store(other._nextDequeue.load(std::memory_order_relaxed), std::memory_order_relaxed);
        _slots = std::move(other._slots);
    }
    return *this;
}

template <typename TData> template <typename T> bool Queue<TData>::Push(T&& data)
{
    // Since we have data dependency here, we can just use relaxed.
    // next is the desired index of the next enqueue.
    size_t next = _nextEnqueue.load(std::memory_order_relaxed);

    Slot* slot;
    for (;;)
    {
        slot = &_slots[next & _mask];
        // Make sure it is the first instruction.
        size_t index = slot->Index.load(std::memory_order_acquire);
        if (next == index)
        {
            // Make sure other thread won't enqueue to the same node.
            if (_nextEnqueue.compare_exchange_weak(next, next + 1, std::memory_order_relaxed))
            {
                break;
            }
        }
        else if (next > index)
        {
            // Surpasses the last available node.
            return false;
        }
        else
        {
            // Oops, this `next` seems to be outdated, update it.
            next = _nextEnqueue.load(std::memory_order_relaxed);
        }
    }

    // Now we have the slot, enqueue the data.
    slot->Data = std::forward<T>(data);
    // Add one to the index to make it available for dequeue.
    // Also, make sure it is the last instruction.
    slot->Index.store(next + 1, std::memory_order_release);

    return true;
}

template <typename TData> bool Queue<TData>::Pop(TData* data)
{
    size_t next = _nextDequeue.load(std::memory_order_relaxed);
    Slot* slot;

    for (;;)
    {
        slot = &_slots[next & _mask];
        size_t index = slot->Index.load(std::memory_order_acquire);
        // Hope that the compiler can optimize `next + 1`.
        if ((next + 1) == index)
        {
            if (_nextDequeue.compare_exchange_weak(next, next + 1, std::memory_order_relaxed))
            {
                break;
            }
        }
        else if ((next + 1) > index)
        {
            return false;
        }
        else
        {
            next = _nextDequeue.load(std::memory_order_relaxed);
        }
    }

    *data = std::move(slot->Data);
    slot->Index.store(next + _capacity, std::memory_order_release);

    return true;
}

} // namespace threading

MINET_END