#include "gx-cr-sync-parallel-for.hpp"
#include "../../platform/gx-plt-log.hpp"
#include "gx-cr-sync-semaphore.hpp"
#include "gx-cr-sync-thread.hpp"
#include <vector>

namespace {
struct Data final {
    gearoenix::core::sync::Semaphore signal {};
    std::atomic<const std::function<void(unsigned int /* count */, unsigned int /* index */)>*> job_function = nullptr;
    std::atomic<gearoenix::core::sync::Semaphore*> job_signal = nullptr;
    std::atomic<bool> is_running = true;
    std::atomic<bool> terminated = false;
    std::thread thread;

    // Only because of some compilers and std implementations, otherwise there was no need for this function
    Data(const Data&) { GX_UNEXPECTED; }

    Data(const unsigned int count, const unsigned int index)
    {
        // It was causing a problem in linux in debug mode
        thread = std::thread([count, index, this] {
            while (is_running) {
                signal.lock();
                if (const auto* const fun = job_function.load(); fun != nullptr) {
                    (*fun)(count, index);
                    job_function = nullptr;
                    (*job_signal).release();
                    job_signal = nullptr;
                }
            }
            terminated = true;
        });
    }

    ~Data()
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
        job_function = function;
        job_signal = sig;
        signal.release();
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
    thread_local std::vector<Data> data = initialise_data();
    thread_local Semaphore signal;
    static const auto count = static_cast<unsigned int>(data.size() + 1);

    for (auto& d : data) {
        d.send_work(&fun, &signal);
    }
    fun(count, 0);
    for (unsigned int index = 1; index < count; ++index) {
        signal.lock();
    }
}