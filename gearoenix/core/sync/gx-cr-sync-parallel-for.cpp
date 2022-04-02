#include "gx-cr-sync-parallel-for.hpp"
#ifndef GX_THREAD_NOT_SUPPORTED
#include "../../platform/gx-plt-log.hpp"
#include "gx-cr-sync-semaphore.hpp"
#include <vector>

struct GearoenixCoreSyncParallelForData final {
    struct Job final {
        const std::function<void(unsigned int, unsigned int)>* const function;
        gearoenix::core::sync::Semaphore* const signal;
    };

    std::thread thread;
    gearoenix::core::sync::Semaphore signal;
    std::mutex jobs_lock;
    std::vector<Job> jobs;
    bool is_running = true;

    GearoenixCoreSyncParallelForData(const unsigned int count, const unsigned int index) noexcept
        : thread([count, index, this] {
            std::vector<Job> local_jobs;
            is_running = true;
            while (is_running) {
                signal.lock();
                {
                    std::lock_guard<std::mutex> _lg(jobs_lock);
                    std::swap(jobs, local_jobs);
                }
                for (const auto& j : local_jobs) {
                    (*j.function)(count, index);
                    j.signal->release();
                }
                local_jobs.clear();
            }
            is_running = true; // shows it has finished running.
        })
    {
    }

    ~GearoenixCoreSyncParallelForData() noexcept
    {
        is_running = false;
        do {
            signal.release();
        } while (!is_running);
        thread.join();
    }

    void send_work(
        const std::function<void(unsigned int, unsigned int)>* const function,
        gearoenix::core::sync::Semaphore* const sig) noexcept
    {
        {
            std::lock_guard<std::mutex> _lg(jobs_lock);
            jobs.push_back({ function, sig });
        }
        signal.release();
    }
};

static std::vector<std::unique_ptr<GearoenixCoreSyncParallelForData>> gearoenix_core_sync_parallel_for_data;

void gearoenix::core::sync::ParallelFor::initialise() noexcept
{
    const auto threads_count = static_cast<unsigned int>(std::thread::hardware_concurrency());
    gearoenix_core_sync_parallel_for_data.reserve(threads_count - 1);
    for (unsigned int thread_index = 1; thread_index < threads_count; ++thread_index) {
        gearoenix_core_sync_parallel_for_data.emplace_back(new GearoenixCoreSyncParallelForData(threads_count, thread_index));
    }
}

void gearoenix::core::sync::ParallelFor::exec(const std::function<void(unsigned int, unsigned int)>& fun) noexcept
{
    Semaphore signal;
    const auto count = static_cast<unsigned int>(gearoenix_core_sync_parallel_for_data.size() + 1);
    for (auto& data : gearoenix_core_sync_parallel_for_data) {
        data->send_work(&fun, &signal);
    }
    fun(count, 0);
    for (unsigned int index = 1; index < count; ++index) {
        signal.lock();
    }
}
#endif