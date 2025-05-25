#include "gx-cr-sync-parallel-for.hpp"
#include "../../platform/gx-plt-log.hpp"
#include "gx-cr-sync-semaphore.hpp"
#include <map>
#include <vector>

namespace {
struct GearoenixCoreSyncParallelForData final {
    struct Job final {
        const std::function<void(unsigned int, unsigned int)>* const function;
        gearoenix::core::sync::Semaphore* const signal;
    };

    gearoenix::core::sync::Semaphore signal {};
    std::mutex jobs_lock {};
    std::vector<Job> jobs;
    bool is_running = true;
    bool terminated = false;
    std::thread thread;

    GearoenixCoreSyncParallelForData(const unsigned int count, const unsigned int index)
    {
        // It was causing problem in linux in debug mode
        thread = std::thread([count, index, this] {
            std::vector<Job> local_jobs;
            is_running = true;
            while (is_running) {
                signal.lock();
                {
                    std::lock_guard _lg(jobs_lock);
                    std::swap(jobs, local_jobs);
                }
                for (const auto [function, signal] : local_jobs) {
                    (*function)(count, index);
                    signal->release();
                }
                local_jobs.clear();
            }
            terminated = true;
        });
    }

    ~GearoenixCoreSyncParallelForData()
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
        gearoenix::core::sync::Semaphore* const sig)
    {
        {
            std::lock_guard _lg(jobs_lock);
            jobs.push_back({ function, sig });
        }
        signal.release();
    }
};

std::mutex gearoenix_core_sync_parallel_for_data_lock;
std::map<std::thread::id, std::vector<std::unique_ptr<GearoenixCoreSyncParallelForData>>> gearoenix_core_sync_parallel_for_data;

[[nodiscard]] std::vector<std::unique_ptr<GearoenixCoreSyncParallelForData>>& gearoenix_core_sync_parallel_for_data_initialise()
{
    const auto id = std::this_thread::get_id();
    {
        std::lock_guard _lg(gearoenix_core_sync_parallel_for_data_lock); // This is super safe and maybe excessive (?)
        const auto search = gearoenix_core_sync_parallel_for_data.find(id);
        if (gearoenix_core_sync_parallel_for_data.end() != search) {
            return search->second;
        }
    }
    const auto tc = gearoenix::core::sync::threads_count();
    std::vector<std::unique_ptr<GearoenixCoreSyncParallelForData>> data;
    data.reserve(tc - 1);
    for (auto thread_index = decltype(tc) { 1 }; thread_index < tc; ++thread_index) {
        data.emplace_back(new GearoenixCoreSyncParallelForData(tc, thread_index));
    }
    std::lock_guard _lg(gearoenix_core_sync_parallel_for_data_lock);
    const auto result = gearoenix_core_sync_parallel_for_data.emplace(id, std::move(data));
    return result.first->second;
}
}

void gearoenix::core::sync::ParallelFor::exec(const std::function<void(unsigned int, unsigned int)>& fun)
{
    const auto& datas = gearoenix_core_sync_parallel_for_data_initialise();
    Semaphore signal;
    const auto count = static_cast<unsigned int>(datas.size() + 1);
    for (const auto& data : datas) {
        data->send_work(&fun, &signal);
    }
    fun(count, 0);
    for (unsigned int index = 1; index < count; ++index) {
        signal.lock();
    }
}