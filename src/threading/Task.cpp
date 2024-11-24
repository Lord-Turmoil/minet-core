#include "minet/threading/Task.h"
#include "minet/common/Assert.h"
#include "minet/threading/Thread.h"

MINET_BEGIN

Task::Task(Ref<Thread> thread) : _thread(std::move(thread))
{
    MINET_ASSERT(_thread);
}

Task::Task() : _thread(nullptr)
{
}

int Task::StartAsync() const
{
    return _thread ? _thread->Start() : 0;
}

int Task::Await() const
{
    return _thread ? _thread->Join() : 0;
}

int Task::Run() const
{
    MINET_TRY(StartAsync());
    return Await();
}

int Task::Cancel() const
{
    return _thread ? _thread->Terminate() : 0;
}

Ref<Task> Task::Create(const Ref<Thread>& thread)
{
    return CreateRef<Task>(thread);
}

Ref<Task> Task::Completed()
{
    return CreateRef<Task>(new Task());
}

MINET_END
