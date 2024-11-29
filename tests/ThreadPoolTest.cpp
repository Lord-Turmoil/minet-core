#include <minet/minet.h>

#include "threading/ThreadPool.h"

#include "doctest.h"

#include <array>
#include <future>

struct task_t
{
    std::packaged_task<int()> task;
    std::future<int> result;
};

void func(task_t* task)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    task->task();
}

TEST_CASE("ThreadPool")
{
    using namespace minet::threading;
    static constexpr int TASK_NUM = 1024;

    std::array<task_t, TASK_NUM> tasks;

    unsigned threads = HardwareConcurrency();
    ThreadPool pool(threads, TASK_NUM / threads);

    task_t* task = tasks.data();
    for (int i = 0; i < TASK_NUM; i++)
    {
        task->task = std::packaged_task<int()>([i]() { return i; });
        task->result = tasks[i].task.get_future();
        pool.Submit([task]() { func(task); });
        task++;
    }

    for (int i = 0; i < TASK_NUM; i++)
    {
        CHECK(tasks[i].result.get() == i);
    }
}
