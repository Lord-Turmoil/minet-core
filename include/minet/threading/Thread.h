#pragma once

#include <functional>
#include <pthread.h>
#include "minet/common/Base.h"

MINET_BEGIN

/**
 * @brief Represents a thread.
 * @note
 * This project uses C++ 17, so we don't yet have jthread with cancellation token.
 * However, it targets Linux only, so we can use pthreads. But pthreads are a little
 * bit low-level, so a wrapper can be beneficial.
 * @note
 * Since we are not implementing a threading library, we don't need to support
 * fancy arguments. Just the very basic functionality is enough.
 * @warning
 * We must ensure that the Thread object dies after the thread is finished.
 */
class Thread final
{
public:
    using ThreadFn = std::function<void(void)>;

    Thread(ThreadFn routine);
    ~Thread();

    /**
     * @brief Start the thread.
     * @return 0 if successful, otherwise an error code.
     */
    int Start();

    /**
     * @brief Wait for the thread to finish.
     * @return 0 if successful, otherwise an error code.
     */
    int Join();

    /**
     * @brief Terminate the thread.
     * @note
     * It will try cancel the thread if it's running.
     * @return 0 if successful, otherwise an error code.
     */
    int Terminate();

    /**
     * @brief Get the thread ID.
     * @brief The thread ID. Or 0 if the thread is not running.
     */
    pthread_t GetThreadId() const
    {
        return _thread;
    }

    /**
     * @brief Whether the thread is started.
     * @return True if the thread is started, otherwise false.
     */
    bool Started() const
    {
        return _thread != 0;
    }

    /**
     * @brief Whether the thread is joinable.
     * Because we don't support detach here, a thread is joinable if it's running.
     * @return True if the thread is joinable, otherwise false.
     */
    bool Joinable() const
    {
        return _thread != 0;
    }

    static Ref<Thread> Create(ThreadFn routine);

private:
    /**
     * @brief Cleanup the thread when it stops.
     */
    void CleanUp();

private:
    // Since pthread routine requries a void* arg, we need to adapt
    // our function signature to match it.
    static void* _ThreadRoutine(void* arg);

    /**
     * Thread id. If 0, the thread is not running.
     */
    pthread_t _thread;

    /**
     * @brief The context for the thread.
     * @note This is the argument to the routine.
     * So the routine can access the underlying task.
     */
    struct ThreadContext
    {
        ThreadFn Routine;
    } _context;
};

MINET_END
