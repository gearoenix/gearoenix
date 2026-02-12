#include "gx-cr-job-manager.hpp"
#include "../gx-cr-function-loader.hpp"
#include "../macro/gx-cr-mcr-assert.hpp"
#include "../sync/gx-cr-sync-semaphore.hpp"
#include "../sync/gx-cr-sync-thread.hpp"

#include <boost/container/flat_map.hpp>
#include <boost/lockfree/queue.hpp>

#include <optional>
#include <queue>
#include <ranges>
#include <thread>

namespace {
enum struct ThreadState {
    Working,
    Finished,
    Terminated,
};

struct ThreadData {
    ThreadState state = ThreadState::Working;
};

struct WorkerData {
    gearoenix::core::FunctionLoader function_loader;
    std::optional<std::thread> thread;
    std::optional<ThreadData> thread_data;
};

std::mutex workers_lock;
boost::container::flat_map<std::thread::id, std::unique_ptr<WorkerData>> workers;

std::mutex tasks_lock;
std::queue<std::function<void()>> tasks;

gearoenix::core::sync::Semaphore pool_signal;
std::size_t pool_size = 0;

[[nodiscard]] std::thread::id register_new_thread()
{
    auto worker = std::make_unique<WorkerData>();
    worker->thread_data.emplace();
    worker->thread.emplace([data = worker.get()] {
        data->thread_data->state = ThreadState::Working;
        while (ThreadState::Working == data->thread_data->state) {
            if (tasks.empty() && data->function_loader.empty()) {
                pool_signal.lock();
            }
            if (ThreadState::Working != data->thread_data->state) {
                break;
            }
            data->function_loader.unload();
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
        data->thread_data->state = ThreadState::Terminated;
    });

    const auto return_value = worker->thread->get_id();

    const std::lock_guard _lg(workers_lock);
    ++pool_size;
    workers[return_value] = std::move(worker);

    return return_value;
}

bool initialised = false;
}

void gearoenix::core::job::initialise()
{
    if (initialised) {
        return;
    }
    initialised = true;

    const auto count = sync::threads_count() * 2;
    for (auto i = 0; i < count; ++i) {
        (void)register_new_thread();
    }

    // pool is ready
}

void gearoenix::core::job::register_current_thread()
{
    register_thread(std::this_thread::get_id());
}

void gearoenix::core::job::register_thread(const std::thread::id thread_id, std::optional<std::thread>&& thread)
{
    auto worker = std::make_unique<WorkerData>();
    worker->thread = std::move(thread);

    const std::lock_guard _lg(workers_lock);
    workers[thread_id] = std::move(worker);
}

void gearoenix::core::job::send_job(const std::thread::id receiver_thread_id, std::function<void()>&& job)
{
    if (std::this_thread::get_id() == receiver_thread_id) {
        job();
        return;
    }
    const auto& search = workers.find(receiver_thread_id);
    if (workers.end() == search) {
        GX_LOG_I("Worker job thread hasn't found for thread-id: " << receiver_thread_id);
        job();
        return;
    }
    auto& worker = *search->second;
    worker.function_loader.load(std::move(job));
    if (worker.thread_data.has_value()) {
        pool_signal.release();
    }
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
    const auto& search = workers.find(std::this_thread::get_id());
    GX_ASSERT(workers.end() != search);
    search->second->function_loader.unload();
}

void gearoenix::core::job::terminate()
{
    for (const auto& worker : workers | std::views::values) {
        if (worker->thread_data.has_value()) {
            if (ThreadState::Terminated != worker->thread_data->state) {
                worker->thread_data->state = ThreadState::Finished;
                while (worker->thread_data->state != ThreadState::Terminated) {
                    pool_signal.release();
                }
            }
        }
        if (worker->thread.has_value()) {
            worker->thread->join();
        }
    }
    workers.clear();
}