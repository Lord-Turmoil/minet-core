/**
 * @author Tony S.
 * @details Task wrapper.
 */

#pragma once

#include "minet/common/Assert.h"

#include <functional>
#include <future>
#include <utility>

MINET_BEGIN

/**
 * @brief Basic task has no return value.
 */
class Task final : public std::enable_shared_from_this<Task>
{
    struct Private
    {
        explicit Private() = default;
    };

public:
    using TaskFn = std::function<void()>;

    explicit Task(TaskFn routine, Private) : _routine(std::move(routine))
    {
    }

    static Ref<Task> Create(const TaskFn& routine)
    {
        return CreateRef<Task>(routine, Private());
    }

    static Ref<Task> Completed()
    {
        return CreateRef<Task>([] {}, Private());
    }

    Ref<Task> StartAsync()
    {
        MINET_ASSERT(!_future.valid()); // not started before
        _future = std::async(std::launch::async, _routine);
        return shared_from_this();
    }

    void Await()
    {
        MINET_ASSERT(_future.valid()); // not started before
        _future.get();
    }

    void Run()
    {
        StartAsync()->Await();
    }

private:
    TaskFn _routine;
    std::future<void> _future;
};

/**
 * @brief ValueTask has return value.
 */
template <typename TResult> class ValueTask final : public std::enable_shared_from_this<ValueTask<TResult>>
{
    struct Private
    {
        explicit Private() = default;
    };

public:
    using ValueTaskFn = std::function<TResult()>;

    explicit ValueTask(ValueTaskFn routine, Private) : _routine(std::move(routine))
    {
    }

    /**
     * @brief Create a task with the given thread.
     */
    static Ref<ValueTask> Create(ValueTaskFn routine)
    {
        return CreateRef<Task>(std::move(routine), Private());
    }

    /**
     * @brief Create a completed task from result.
     */
    static Ref<ValueTask> Completed(const TResult& result)
    {
        return CreateRef<Task>([result]() -> TResult { return result; }, Private());
    }

    /**
     * @brief Start the task asynchronously.
     * @return The task itself.
     */
    Ref<ValueTask> StartAsync()
    {
        MINET_ASSERT(!_future.valid()); // not started before
        _future = std::async(std::launch::async, _routine);
        return this->shared_from_this();
    }

    /**
     * @brief Wait for the task to finish.
     * @return Result of the task.
     */
    TResult Await()
    {
        MINET_ASSERT(_future.valid()); // not started before
        return _future.get();
    }

    TResult Run()
    {
        return StartAsync().Await();
    }

private:
    ValueTaskFn _routine;
    std::future<TResult> _future;
};

MINET_END
