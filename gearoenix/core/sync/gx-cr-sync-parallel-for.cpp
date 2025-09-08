#include "gx-cr-sync-parallel-for.hpp"

#include "../macro/gx-cr-mcr-assert.hpp"
#include "gx-cr-sync-semaphore.hpp"
#include "gx-cr-sync-thread.hpp"

#include <boost/container/flat_map.hpp>

#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

namespace {
struct ThreadsManager final {
private:
    std::atomic<int> running = 0;
    std::atomic<int> terminated = 0;
    std::atomic<const gearoenix::core::sync::parallel_for_t*> fun = nullptr;
    std::vector<std::unique_ptr<gearoenix::core::sync::Semaphore>> start_signals;
    gearoenix::core::sync::Semaphore finish_signal;
    std::vector<std::thread> threads;
    const unsigned int threads_count;

public:
    ThreadsManager()
        : threads_count(gearoenix::core::sync::threads_count())
    {
        start_signals.reserve(threads_count);
        for (auto thread_index = decltype(threads_count) { 0 }; thread_index < threads_count; ++thread_index) {
            start_signals.push_back(std::make_unique<gearoenix::core::sync::Semaphore>());
        }
        threads.reserve(threads_count);
        for (auto thread_index = decltype(threads_count) { 0 }; thread_index < threads_count; ++thread_index) {
            threads.emplace_back([this, thread_index = thread_index] {
                auto& start_signal = *start_signals[thread_index];
                ++running;
                while (0 < running.load()) {
                    start_signal.lock();
                    if (0 >= running.load()) {
                        break;
                    }
                    const auto* const f = fun.load();
                    GX_ASSERT_D(f);
                    (*f)(thread_index, threads_count);
                    finish_signal.release();
                }
                ++terminated;
            });
        }
        std::this_thread::yield();
        while (static_cast<int>(threads.size()) > running.load()) {
            std::this_thread::yield();
        }
    }

    ~ThreadsManager()
    {
        do {
            running = 0;
            for (const auto& s : start_signals) {
                s->release();
            }
            std::this_thread::yield();
        } while (static_cast<int>(threads.size()) > terminated.load());

        for (auto& t : threads) {
            t.join();
        }
    }

    void run(const gearoenix::core::sync::parallel_for_t* const f)
    {
        GX_ASSERT_D(nullptr == fun.load()); // to make sure no other function is running
        fun = f;
        for (const auto& s : start_signals) {
            s->release();
        }
        for (auto i = decltype(threads_count) { 0 }; i < threads_count; ++i) {
            finish_signal.lock();
        }
        fun = nullptr;
    }
};

std::mutex threads_managers_lock;
boost::container::flat_map<std::thread::id, std::unique_ptr<ThreadsManager>> threads_managers;
}

void gearoenix::core::sync::parallel(parallel_for_t* const fun)
{
    [] {
        std::lock_guard _lg(threads_managers_lock);
        auto& m = threads_managers[std::this_thread::get_id()];
        if (!m) {
            m = std::make_unique<ThreadsManager>();
        }
        return m.get();
    }()
        ->run(fun);
}
