#include "gx-cr-sync-parallel-for.hpp"

#include "../job/gx-cr-job-manager.hpp"
#include "gx-cr-sync-semaphore.hpp"
#include "gx-cr-sync-thread.hpp"

namespace {

}

void gearoenix::core::sync::parallel(const parallel_for_t& fun)
{
    static const auto count = threads_count();
    Semaphore end;

    for (auto i = decltype(count) { 1 }; i < count; ++i) {
        job::send_job_to_pool([&, thread_index = i] {
            fun(thread_index, count);
            end.release();
        });
    }

    fun(0, count);

    for (auto i = decltype(count) { 1 }; i < count; ++i) {
        end.lock();
    }
}
