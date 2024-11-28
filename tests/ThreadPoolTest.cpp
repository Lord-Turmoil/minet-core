#include <minet/minet.h>

#include "threading/ThreadPool.h"

#include "doctest.h"

#include <future>

TEST_CASE("ThreadPool")
{
    using namespace minet::threading;
    static constexpr int TASK_NUM = 1024;
    std::future<int> results[TASK_NUM];
    std::packaged_task<int()> tasks[TASK_NUM];

    // Make it busier?
    unsigned int threads = HardwareConcurrency();
    ThreadPool pool(threads, TASK_NUM / threads);
    for (int i = 0; i < TASK_NUM; i++)
    {
        tasks[i] = std::packaged_task<int()>([i]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            return i;
        });
        results[i] = tasks[i].get_future();
    }

    for (int i = 0; i < TASK_NUM; i++)
    {
        // clang-format off
        CHECK_EQ(pool.Submit([&tasks, i]() {
            tasks[i]();
        }), true);
        // clang-format on
    }

    for (int i = 0; i < TASK_NUM; i++)
    {
        CHECK_EQ(results[i].get(), i);
    }
}
