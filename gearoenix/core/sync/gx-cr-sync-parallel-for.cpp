#include "gx-cr-sync-parallel-for.hpp"
#include "../../platform/gx-plt-log.hpp"
#include "gx-cr-sync-semaphore.hpp"
#include "gx-cr-sync-thread.hpp"
#include <vector>

namespace {
struct Data final {
    typedef const std::function<void(unsigned int /* count */, unsigned int /* index */)> job_t;

    gearoenix::core::sync::Semaphore job_available_signal;
    gearoenix::core::sync::Semaphore job_finished_signal;
    job_t* job;
    bool is_running = false;
    bool is_terminated = false;
    std::thread thread;

    // Only because of some compilers and std implementations, otherwise there was no need for this function
    Data(const Data&) { GX_UNEXPECTED; }

    Data(const unsigned int count, const unsigned int index)
    {
        thread = std::thread([count, index, this] {
            job = nullptr;
            is_terminated = false;
            is_running = true;

            while (is_running) {
                job_available_signal.lock();
                if (!is_running) {
                    break;
                }
                (*job)(count, index);
                job = nullptr;
                job_finished_signal.release();
            }

            is_terminated = true;
        });
        do {
            std::this_thread::yield();
        } while (!is_running);
    }

    ~Data()
    {
        do {
            is_running = false;
            job_available_signal.release();
            std::this_thread::yield();
        } while (!is_terminated || !thread.joinable());
        thread.join();
    }

    void send_work(const std::function<void(unsigned int, unsigned int)>& function)
    {
        job = &function;
        job_available_signal.release();
    }

    void wait_for_job_to_finish()
    {
        job_finished_signal.lock();
    }
};

[[nodiscard]] std::vector<Data> initialise_data()
{
    const auto tc = static_cast<unsigned int>(gearoenix::core::sync::threads_count());
    std::vector<Data> data;
    data.reserve(tc - 1);
    for (auto thread_index = decltype(tc) { 1 }; thread_index < tc; ++thread_index) {
        data.emplace_back(tc, thread_index);
    }
    return data;
}
}

void gearoenix::core::sync::ParallelFor::exec(const std::function<void(unsigned int, unsigned int)>& fun)
{
    thread_local std::vector<Data> data;
    if (data.empty()) {
        data = initialise_data();
    }
    for (auto& d : data) {
        d.send_work(fun);
    }
    fun(static_cast<unsigned int>(data.size() + 1), 0);
    for (auto& d : data) {
        d.wait_for_job_to_finish();
    }
}