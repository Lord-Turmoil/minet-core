#include "minet/threading/Thread.h"
#include "minet/common/Assert.h"

MINET_BEGIN

Thread::Thread(ThreadFn routine) : _thread(0)
{
    _context.Routine = std::move(routine);
}

Thread::~Thread()
{
    _CleanUp();
}

Ref<Thread> Thread::Create(const ThreadFn& routine)
{
    return CreateRef<Thread>(routine);
}

int Thread::Start()
{
    if (_thread)
    {
        return -1;
    }

    return pthread_create(&_thread, nullptr, _ThreadRoutine, &_context);
}

int Thread::Join()
{
    if (!_thread)
    {
        return -1;
    }

    int ret = pthread_join(_thread, nullptr);
    MINET_ASSERT(ret == 0, "Failed to join thread");
    _thread = 0;
    return ret;
}

int Thread::Terminate()
{
    if (!_thread)
    {
        return -1;
    }

    int ret = pthread_cancel(_thread);
    MINET_ASSERT(ret == 0, "Failed to cancel thread");
    _thread = 0;
    return ret;
}

void Thread::_CleanUp()
{
    if (_thread)
    {
        Terminate();
    }
}

void* Thread::_ThreadRoutine(void* arg)
{
    ThreadContext* context = static_cast<ThreadContext*>(arg);
    MINET_ASSERT(context->Routine);
    context->Routine();
    return nullptr;
}

MINET_END
