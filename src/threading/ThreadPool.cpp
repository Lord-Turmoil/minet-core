#include "threading/ThreadPool.h"

MINET_BEGIN

namespace threading
{

unsigned int HardwareConcurrency()
{
    return std::thread::hardware_concurrency();
}

ThreadPool::ThreadPool(unsigned int threads, size_t capacity)
    : _threads(threads), _running(false), _workers(threads), _next(0)
{
    MINET_ASSERT(threads > 0);

    for (unsigned int i = 0; i < threads; i++)
    {
        _workers[i] = CreateRef<Worker<TaskFn>>(capacity);
    }

    for (unsigned int i = 0; i < _threads; i++)
    {
        _workers[i]->Start(_workers[(i + 1) % _threads].get());
    }
}

ThreadPool::~ThreadPool()
{
    for (unsigned int i = 0; i < _threads; i++)
    {
        _workers[i]->Stop();
    }
}

ThreadPool::ThreadPool(ThreadPool&& other) noexcept
{
    _threads = other._threads;
    _running = other._running;
    _workers = std::move(other._workers);
    _next.store(other._next.load());
}

ThreadPool& ThreadPool::operator=(ThreadPool&& other) noexcept
{
    if (this != &other)
    {
        _threads = other._threads;
        _running = other._running;
        _workers = std::move(other._workers);
        _next.store(other._next.load());
    }
    return *this;
}

} // namespace threading

MINET_END
