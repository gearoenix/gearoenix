#include "gx-cr-sync-parallel-for.hpp"
#include "../macro/gx-cr-mcr-assert.hpp"
#include "gx-cr-sync-thread.hpp"

#include <atomic>
#include <new>
#include <optional>
#include <thread>
#include <vector>

namespace {
struct ParallelContext final {
    alignas(std::hardware_destructive_interference_size) std::atomic<unsigned int> generation;
    alignas(std::hardware_destructive_interference_size) std::atomic<unsigned int> done_count = 0;

    const unsigned int count;
    std::atomic<unsigned int> terminated_count = 0;
    gearoenix::core::sync::parallel_for_t task = nullptr;
    const void* context = nullptr;
    std::vector<std::thread> workers;

    ParallelContext()
        : generation(gearoenix::core::sync::threads_count())
        , count(gearoenix::core::sync::threads_count())
    {
        workers.reserve(count);
        for (std::uint32_t i = 0; i < count; ++i) {
            workers.emplace_back([this] {
                thread_func();
            });
        }
    }

    void thread_func()
    {
        auto index = count;
        while (true) {
            generation.wait(index, std::memory_order_acquire);
            index = generation.load(std::memory_order_relaxed);
            if (index >= count) {
                continue;
            }
            index = generation.fetch_add(1, std::memory_order_relaxed);
            if (index >= count) {
                continue;
            }
            if (task == nullptr) {
                break;
            }
            task(index, static_cast<unsigned int>(count), context);
            done_count.fetch_add(1, std::memory_order_release);
            done_count.notify_one();
        }
        terminated_count.fetch_add(1, std::memory_order_release);
        terminated_count.notify_all();
    }

    ~ParallelContext()
    {
        GX_ASSERT_D(terminated_count.load(std::memory_order_relaxed) == 0); // One or more threads in ParallelContext is dead already!
        task = nullptr;
        context = nullptr;

        for (auto ti = terminated_count.load(std::memory_order_acquire); ti < count; ti = terminated_count.load(std::memory_order_acquire)) {
            generation.store(0, std::memory_order_release);
            generation.notify_all();
            std::this_thread::yield();
            terminated_count.wait(ti, std::memory_order_acquire);
            std::this_thread::yield();
        }

        for (auto& w : workers) {
            std::this_thread::yield();
            w.detach(); // In some platforms, because of `thread_local` implementation, joining thread here can cause deadlock.
            std::this_thread::yield();
        }
    }

    void run(const gearoenix::core::sync::parallel_for_t fun, const void* const fun_data)
    {
        GX_ASSERT_D(terminated_count.load(std::memory_order_relaxed) == 0); // One or more threads in ParallelContext is dead already!
        task = fun;
        context = fun_data;
        done_count.store(0, std::memory_order_relaxed);
        generation.store(0, std::memory_order_release);
        generation.notify_all();
        for (auto i = done_count.load(std::memory_order_relaxed); i < count; i = done_count.load(std::memory_order_relaxed)) {
            done_count.wait(i, std::memory_order_acquire);
        }
    }
};

thread_local std::optional<ParallelContext> ctx;
}

void gearoenix::core::sync::parallel(const parallel_for_t fun, const void* const fun_data)
{
    if (!ctx.has_value()) {
        ctx.emplace();
    }
    ctx->run(fun, fun_data);
}