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

int Task::StartAsync()
{
    return _thread ? _thread->Start() : 0;
}

int Task::Await()
{
    return _thread ? _thread->Join() : 0;
}

int Task::Run()
{
    int ret = StartAsync();
    if (ret)
    {
        return ret;
    }
    return Await();
}

int Task::Cancel()
{
    return _thread ? _thread->Terminate() : 0;
}

Ref<Task> Task::Create(Ref<Thread> thread)
{
    return CreateRef<Task>(thread);
}

Ref<Task> Task::Completed()
{
    return CreateRef<Task>(new Task());
}

MINET_END
