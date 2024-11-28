/**
 * @author Tony S.
 * @details Task wrapper.
 * @note
 * Simply using std::future with std::async may cause data races.
 * So, we take advantage of std::packaged_task to wrap the task.
 */

#pragma once

#include <functional>
#include <future>
#include <utility> // std::forward

MINET_BEGIN

namespace threading
{

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

    Task(TaskFn routine, Private);

    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;
    Task(Task&& other) noexcept;
    Task& operator=(Task&& other) noexcept;

    template <typename TRoutine> static Ref<Task> Create(TRoutine&& routine);
    static Ref<Task> Completed();

    Ref<Task> StartAsync();
    void Await();
    void Run();

private:
    std::packaged_task<void()> _task;
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

    ValueTask(ValueTaskFn routine, Private);

    ValueTask(const ValueTask& other) = delete;
    ValueTask& operator=(const ValueTask& other) = delete;
    ValueTask(ValueTask&& other) noexcept;
    ValueTask& operator=(ValueTask&& other) noexcept;

    template <typename TRoutine> static Ref<ValueTask> Create(TRoutine&& routine);
    static Ref<ValueTask> Completed(const TResult& result);

    Ref<ValueTask> StartAsync();
    TResult Await();
    TResult Run();

private:
    std::packaged_task<TResult()> _task;
    std::future<TResult> _future;
};

/*
 * ===================================================================
 * ---------------------- Task Implementation ------------------------
 * ===================================================================
 */
inline Task::Task(TaskFn routine, Private) : _task(std::move(routine))
{
}

inline Task::Task(Task&& other) noexcept : _task(std::move(other._task)), _future(std::move(other._future))
{
}

inline Task& Task::operator=(Task&& other) noexcept
{
    if (this != &other)
    {
        _task = std::move(other._task);
        _future = std::move(other._future);
    }
    return *this;
}

template <typename TRoutine> Ref<Task> Task::Create(TRoutine&& routine)
{
    return CreateRef<Task>(std::forward<TRoutine>(routine), Private());
}

inline Ref<Task> Task::Completed()
{
    return CreateRef<Task>([] {}, Private());
}

inline Ref<Task> Task::StartAsync()
{
    _future = _task.get_future();
    _task();
    return shared_from_this();
}

inline void Task::Await()
{
    if (!_future.valid())
    {
        throw std::runtime_error("Task not started");
    }
    _future.get();
}

inline void Task::Run()
{
    StartAsync()->Await();
}

/*
 * ===================================================================
 * --------------------- ValueTask Implementation --------------------
 * ===================================================================
 */

template <typename TResult> ValueTask<TResult>::ValueTask(ValueTaskFn routine, Private) : _task(std::move(routine))
{
}

template <typename TResult>
ValueTask<TResult>::ValueTask(ValueTask&& other) noexcept
    : _task(std::move(other._task)), _future(std::move(other._future))
{
}

template <typename TResult> ValueTask<TResult>& ValueTask<TResult>::operator=(ValueTask&& other) noexcept
{
    if (this != &other)
    {
        _task = std::move(other._task);
        _future = std::move(other._future);
    }
    return *this;
}

template <typename TResult>
template <typename TRoutine>
Ref<ValueTask<TResult>> ValueTask<TResult>::Create(TRoutine&& routine)
{
    return CreateRef<ValueTask>(std::forward<TRoutine>(routine), Private());
}

template <typename TResult> Ref<ValueTask<TResult>> ValueTask<TResult>::Completed(const TResult& result)
{
    return CreateRef<ValueTask>([result] { return result; }, Private());
}

template <typename TResult> Ref<ValueTask<TResult>> ValueTask<TResult>::StartAsync()
{
    _future = _task.get_future();
    _task();
    return this->shared_from_this();
}

template <typename TResult> TResult ValueTask<TResult>::Await()
{
    if (!_future.valid())
    {
        throw std::runtime_error("Task not started");
    }
    return _future.get();
}

template <typename TResult> TResult ValueTask<TResult>::Run()
{
    return StartAsync()->Await();
}

} // namespace threading

MINET_END
