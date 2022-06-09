#include "gx-cr-sync-parallel-for.hpp"
#ifndef GX_THREAD_NOT_SUPPORTED
#include "../../platform/gx-plt-log.hpp"
#include "gx-cr-sync-semaphore.hpp"
#include <map>
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
    bool terminated = false;

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
                for (const auto [function, signal] : local_jobs) {
                    (*function)(count, index);
                    signal->release();
                }
                local_jobs.clear();
            }
            terminated = true;
        })
    {
    }

    ~GearoenixCoreSyncParallelForData() noexcept
    {
        do {
            is_running = false;
            signal.release();
            std::this_thread::yield();
        } while (!terminated || !thread.joinable());
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

static std::mutex gearoenix_core_sync_parallel_for_data_lock;
static std::map<std::thread::id, std::vector<std::unique_ptr<GearoenixCoreSyncParallelForData>>> gearoenix_core_sync_parallel_for_data;

static std::vector<std::unique_ptr<GearoenixCoreSyncParallelForData>>& gearoenix_core_sync_parallel_for_data_initialise() noexcept
{
    const auto id = std::this_thread::get_id();
    {
        std::lock_guard<std::mutex> _lg(gearoenix_core_sync_parallel_for_data_lock); // This is super safe and maybe excessive (?)
        auto search = gearoenix_core_sync_parallel_for_data.find(id);
        if (gearoenix_core_sync_parallel_for_data.end() != search)
            return search->second;
    }
    const auto threads_count = static_cast<unsigned int>(std::thread::hardware_concurrency());
    std::vector<std::unique_ptr<GearoenixCoreSyncParallelForData>> data;
    data.reserve(threads_count - 1);
    for (unsigned int thread_index = 1; thread_index < threads_count; ++thread_index) {
        data.emplace_back(new GearoenixCoreSyncParallelForData(threads_count, thread_index));
    }
    std::lock_guard<std::mutex> _lg(gearoenix_core_sync_parallel_for_data_lock);
    auto result = gearoenix_core_sync_parallel_for_data.emplace(id, std::move(data));
    return result.first->second;
}

void gearoenix::core::sync::ParallelFor::exec(const std::function<void(unsigned int, unsigned int)>& fun) noexcept
{
    auto& datas = gearoenix_core_sync_parallel_for_data_initialise();
    Semaphore signal;
    const auto count = static_cast<unsigned int>(datas.size() + 1);
    for (auto& data : datas) {
        data->send_work(&fun, &signal);
    }
    fun(count, 0);
    for (unsigned int index = 1; index < count; ++index) {
        signal.lock();
    }
}
#endif