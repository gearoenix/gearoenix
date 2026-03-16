#include "gx-cr-job-manager.hpp"
#include "../gx-cr-function-loader.hpp"
#include "../macro/gx-cr-mcr-assert.hpp"
#include "../sync/gx-cr-sync-semaphore.hpp"
#include "../sync/gx-cr-sync-thread.hpp"

#include <boost/container/flat_map.hpp>

#include <optional>
#include <queue>
#include <thread>

namespace {
enum struct ThreadState {
    Working,
    Finished,
    Terminated,
};

struct RegisteredWorkerData {
    gearoenix::core::FunctionLoader function_loader;
    std::optional<std::thread> thread;
};

struct PoolWorkerData {
    std::thread thread;
    ThreadState state = ThreadState::Working;
};

std::mutex registered_workers_lock;
boost::container::flat_map<std::thread::id, std::unique_ptr<RegisteredWorkerData>> registered_workers;

std::mutex tasks_lock;
std::queue<std::function<void()>> tasks;

std::vector<PoolWorkerData> pool_workers;

gearoenix::core::sync::Semaphore pool_signal;

void create_new_thread_for_pool()
{
    const auto index = pool_workers.size();
    pool_workers.emplace_back(std::thread([index] {
        pool_workers[index].state = ThreadState::Working;
        while (ThreadState::Working == pool_workers[index].state) {
            if (tasks.empty()) {
                pool_signal.lock();
            }
            if (ThreadState::Working != pool_workers[index].state) {
                break;
            }
            std::function<void()> task;
            {
                const std::lock_guard _lg(tasks_lock);
                if (tasks.empty()) {
                    continue;
                }
                task = std::move(tasks.front());
                tasks.pop();
            }
            task();
        }
        pool_workers[index].state = ThreadState::Terminated;
    }));
}
}

void gearoenix::core::job::initialise()
{
    static std::atomic initialised = false;
    if (initialised.exchange(true, std::memory_order_relaxed)) {
        return;
    }

    const auto count = sync::threads_count() * 2;
    pool_workers.reserve(count);
    for (auto i = 0; i < count; ++i) {
        create_new_thread_for_pool();
    }
}

void gearoenix::core::job::register_current_thread()
{
    register_thread(std::this_thread::get_id());
}

void gearoenix::core::job::register_thread(const std::thread::id thread_id, std::optional<std::thread>&& thread)
{
    auto worker = std::make_unique<RegisteredWorkerData>();
    worker->thread = std::move(thread);

    const std::lock_guard _lg(registered_workers_lock);
    registered_workers[thread_id] = std::move(worker);
}

void gearoenix::core::job::send_job(const std::thread::id receiver_thread_id, std::function<void()>&& job)
{
    if (std::this_thread::get_id() == receiver_thread_id) {
        job();
        return;
    }
    const auto& search = registered_workers.find(receiver_thread_id);
    if (registered_workers.end() == search) {
        send_job_to_pool(std::move(job));
        return;
    }
    search->second->function_loader.load(std::move(job));
}

void gearoenix::core::job::send_job_to_pool(std::function<void()>&& job)
{
    {
        std::lock_guard _lg(tasks_lock);
        tasks.push(std::move(job));
    }
    pool_signal.release();
}

void gearoenix::core::job::execute_current_thread_jobs()
{
    const auto& search = registered_workers.find(std::this_thread::get_id());
    GX_ASSERT(registered_workers.end() != search);
    search->second->function_loader.unload();
}

void gearoenix::core::job::terminate()
{
    for (auto& [thread, state] : pool_workers) {
        if (ThreadState::Terminated != state) {
            state = ThreadState::Finished;
            while (state != ThreadState::Terminated) {
                pool_signal.release();
            }
        }
        thread.join();
    }
    pool_workers.clear();
    registered_workers.clear();
}