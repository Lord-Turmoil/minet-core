#include "minet/common/Base.h"
#include "minet/threading/Thread.h"

MINET_BEGIN

/**
 * @brief Mimics the Task in C#.
 */
class Task final
{
public:
    /**
     * @brief Create a task with the given thread.
     */
    Task(Ref<Thread> thread);

    /**
     * @brief Start the task asynchronously.
     * @return 0 if successful, otherwise an error code.
     */
    int StartAsync();

    /**
     * @brief Wait for the task to finish.
     * @return 0 if successful, otherwise an error code.
     */
    int Await();

    /**
     * @brief Run the task synchronously.
     * @return 0 if successful, otherwise an error code.
     */
    int Run();

    /**
     * @brief Cancel the task if it's running.
     * @return 0 if successful, otherwise an error code.
     */
    int Cancel();

    /**
     * @brief Create a task with the given thread.
     */
    static Ref<Task> Create(Ref<Thread> thread);

    /**
     * @brief Create a completed task.
     */
    static Ref<Task> Completed();

private:
    /**
     * @brief Create a empty task.
     * This task is not runnable, and represents a completed task in case
     * placeholder is required.
     */
    Task();

private:
    Ref<Thread> _thread;
};

MINET_END
