#pragma once

#include <functional>
#include <future>
#include <utility>
#include "minet/common/Assert.h"
#include "minet/common/Base.h"

MINET_BEGIN

/**
 * @brief Basic task has no return value.
 */
class Task final : std::enable_shared_from_this<Task>
{
public:
    using TaskFn = std::function<void()>;

    static Ref<Task> Create(const TaskFn& routine)
    {
        return CreateRef<Task>(new Task(routine));
    }

    static Ref<Task> Completed()
    {
        return CreateRef<Task>(new Task([] {}));
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
    explicit Task(TaskFn routine) : _routine(std::move(routine))
    {
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
public:
    using ValueTaskFn = std::function<TResult()>;

    /**
     * @brief Create a task with the given thread.
     */
    static Ref<ValueTask> Create(ValueTaskFn routine)
    {
        return CreateRef<Task>(new Task(std::move(routine)));
    }

    /**
     * @brief Create a completed task from result.
     */
    static Ref<ValueTask> Completed(const TResult& result)
    {
        return CreateRef<Task>(new Task([result]() -> TResult { return result; }));
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
    explicit ValueTask(ValueTaskFn routine) : _routine(std::move(routine))
    {
    }

private:
    ValueTaskFn _routine;
    std::future<TResult> _future;
};

MINET_END
