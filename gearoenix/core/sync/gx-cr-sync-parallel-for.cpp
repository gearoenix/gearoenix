#include "gx-cr-sync-parallel-for.hpp"
#include "../job/gx-cr-job-manager.hpp"
#include "gx-cr-sync-semaphore.hpp"
#include "gx-cr-sync-thread.hpp"

#include <latch>

void gearoenix::core::sync::parallel(const parallel_for_t& fun)
{
    static const auto count = threads_count();
    std::latch done(count - 1);

    for (auto i = decltype(count) { 1 }; i < count; ++i) {
        job::send_job_to_pool([&, thread_index = i] {
            fun(thread_index, count);
            done.count_down();
        });
    }

    fun(0, count);

    done.wait();
}