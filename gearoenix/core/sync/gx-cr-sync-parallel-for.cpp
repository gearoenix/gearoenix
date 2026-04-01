#include "gx-cr-sync-parallel-for.hpp"
#include "gx-cr-sync-thread.hpp"

#include <barrier>
#include <thread>
#include <vector>

namespace {
struct ParallelContext final {
    const std::int32_t count;
    gearoenix::core::sync::parallel_for_t* current_task = nullptr;
    std::barrier<> start_barrier;
    std::barrier<> done_barrier;
    std::vector<std::jthread> workers;

    ParallelContext()
        : count(gearoenix::core::sync::threads_count())
        , start_barrier(count + 1)
        , done_barrier(count + 1)
    {
        workers.reserve(count);
        for (std::int32_t i = 0; i < count; ++i) {
            workers.emplace_back([this, index = static_cast<unsigned int>(i)](const std::stop_token& stop) {
                while (true) {
                    start_barrier.arrive_and_wait();
                    if (stop.stop_requested()) {
                        return;
                    }
                    (*current_task)(index, static_cast<unsigned int>(count));
                    done_barrier.arrive_and_wait();
                }
            });
        }
    }

    ~ParallelContext()
    {
        for (auto& w : workers) {
            w.request_stop();
            w.detach();
        }
        start_barrier.arrive_and_wait();
    }

    void run(gearoenix::core::sync::parallel_for_t&& fun)
    {
        current_task = &fun;
        start_barrier.arrive_and_wait();
        done_barrier.arrive_and_wait();
    }
};
}

void gearoenix::core::sync::parallel(parallel_for_t&& fun)
{
    thread_local ParallelContext ctx;
    ctx.run(std::move(fun));
}