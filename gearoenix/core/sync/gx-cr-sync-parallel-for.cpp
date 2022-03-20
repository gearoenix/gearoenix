#include "gx-cr-sync-parallel-for.hpp"
#ifndef GX_THREAD_NOT_SUPPORTED
#include "../../platform/gx-plt-log.hpp"
#include "gx-cr-sync-kernel-workers.hpp"

gearoenix::core::sync::ParallelFor::Map gearoenix::core::sync::ParallelFor::threads;

void gearoenix::core::sync::ParallelFor::exec(std::function<void(unsigned int, unsigned int)> fun) noexcept
{
    const auto id = std::this_thread::get_id();
    auto search = threads.find(id);
    if (threads.end() == search) {
        bool is_ok;
        std::tie(search, is_ok) = threads.emplace(id, std::make_pair(std::make_unique<KernelWorkers>(), std::move(fun)));
#ifdef GX_DEBUG_MODE
        if (!is_ok)
            GX_UNEXPECTED;
#else
        (void)is_ok;
#endif
        auto& kernel_workers = *search->second.first;
        kernel_workers.add_step([kernel_counts { kernel_workers.get_threads_count() },
                                    pair_ptr { &(search->second) }](const unsigned int kernel_index) {
            pair_ptr->second(kernel_counts, kernel_index);
        });
    } else {
        search->second.second = std::move(fun);
    }
    search->second.first->do_steps();
}
#endif