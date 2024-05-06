#include "gx-cr-job-manager.hpp"
#include "../gx-cr-function-loader.hpp"
#include "../macro/gx-cr-mcr-assert.hpp"
#include "../sync/gx-cr-sync-semaphore.hpp"

#include <boost/container/flat_map.hpp>

#include <optional>
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
    gearoenix::core::sync::Semaphore signal;
};

struct WorkerData {
    gearoenix::core::FunctionLoader function_loader;
    std::optional<std::thread> thread;
    std::optional<ThreadData> thread_data;
};

std::mutex workers_lock;
boost::container::flat_map<std::thread::id, std::unique_ptr<WorkerData>> workers;

std::thread::id io1_thread_id;
WorkerData* io1_worker = nullptr;

std::thread::id net1_thread_id;
WorkerData* net1_worker = nullptr;
}

void gearoenix::core::job::initialise()
{
    io1_thread_id = register_new_thread();
    net1_thread_id = register_new_thread();

    io1_worker = workers.find(io1_thread_id)->second.get();
    net1_worker = workers.find(net1_thread_id)->second.get();
}

void gearoenix::core::job::register_current_thread()
{
    register_thread(std::this_thread::get_id());
}

void gearoenix::core::job::register_thread(std::thread::id thread_id, std::optional<std::thread>&& thread)
{
    auto worker = std::make_unique<WorkerData>();
    worker->thread = std::move(thread);

    const std::lock_guard _lg(workers_lock);
    workers.emplace(thread_id, std::move(worker));
}

std::thread::id gearoenix::core::job::register_new_thread()
{
    auto worker = std::make_unique<WorkerData>();
    worker->thread_data.emplace();
    worker->thread = std::thread([data = worker.get()] {
        data->thread_data->state = ThreadState::Working;
        while (ThreadState::Working == data->thread_data->state) {
            data->thread_data->signal.lock();
            if (ThreadState::Working != data->thread_data->state) {
                break;
            }
            data->function_loader.unload();
        }
        data->thread_data->state = ThreadState::Terminated;
    });

    auto return_value = worker->thread->get_id();

    const std::lock_guard _lg(workers_lock);
    workers.emplace(return_value, std::move(worker));

    return return_value;
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
        worker.thread_data->signal.release();
    }
}

void gearoenix::core::job::send_job_io1(std::function<void()>&& job)
{
    if (std::this_thread::get_id() == io1_thread_id) {
        job();
    } else {
        io1_worker->function_loader.load(std::move(job));
        io1_worker->thread_data->signal.release();
    }
}

void gearoenix::core::job::send_job_net1(std::function<void()>&& job)
{
    if (std::this_thread::get_id() == net1_thread_id) {
        job();
    } else {
        net1_worker->function_loader.load(std::move(job));
        net1_worker->thread_data->signal.release();
    }
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
                    worker->thread_data->signal.release();
                }
            }
        }
        if (worker->thread.has_value()) {
            worker->thread->join();
        }
    }
    workers.clear();
}