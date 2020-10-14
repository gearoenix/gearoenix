#include "gx-cr-sync-channeled-workers.hpp"
#ifndef GX_THREAD_NOT_SUPPORTED
#include "../../system/gx-sys-log.hpp"

void gearoenix::core::sync::ChanneledWorkers::Thread::kernel() noexcept
{
    running = true;
    while (running) {
        signal.lock();
        pending_lock.lock();
        std::swap(pending, underprocess);
        pending_lock.unlock();
        underprocess_index = 0;
        for (const auto& f : underprocess) {
            ++underprocess_index;
            f(thread_index);
        }
        underprocess_index = 0;
        underprocess.clear();
    }
    // For signalling the terminator.
    running = true;
}

gearoenix::core::sync::ChanneledWorkers::Thread::Thread(const std::size_t i) noexcept
    : thread_index(i)
    , thread([this] { kernel(); })
{
}

gearoenix::core::sync::ChanneledWorkers::Thread::~Thread() noexcept
{
    running = false;
    if (!running)
        signal.release();
    thread.join();
}

gearoenix::core::sync::ChanneledWorkers::ChanneledWorkers() noexcept
{
    std::size_t count = std::thread::hardware_concurrency();
    threads.reserve(count);
    for (std::size_t i = 0; i < count; ++i)
        threads.emplace_back(new Thread(i));
}

gearoenix::core::sync::ChanneledWorkers::~ChanneledWorkers() noexcept = default;

void gearoenix::core::sync::ChanneledWorkers::perform(const std::function<void(std::size_t)>& job) noexcept
{
    std::size_t min = std::numeric_limits<std::size_t>::max();
    std::size_t i = 0;
    for (const auto& t : threads) {
        std::size_t p = t->pending.size();
        p += t->underprocess.size();
        p -= t->underprocess_index;
        if (p < min) {
            min = p;
            i = t->thread_index;
        }
    }
    auto* const t = threads[i].get();
    std::lock_guard<std::mutex> _lg(t->pending_lock);
    t->pending.push_back(job);
    t->signal.release();
}
#endif
