#include "gx-cr-sync-channeled-workers.hpp"
#include "../../platform/gx-plt-log.hpp"
#include "gx-cr-sync-thread.hpp"

void gearoenix::core::sync::ChanneledWorkers::Thread::kernel()
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

gearoenix::core::sync::ChanneledWorkers::Thread::Thread(const std::uint32_t i)
    : thread_index(i)
    , thread([this] { kernel(); })
{
}

gearoenix::core::sync::ChanneledWorkers::Thread::~Thread()
{
    running = false;
    if (!running)
        signal.release();
    thread.join();
}

gearoenix::core::sync::ChanneledWorkers::ChanneledWorkers()
{
    std::uint32_t count = threads_count();
    threads.reserve(count);
    for (std::uint32_t i = 0; i < count; ++i) {
        threads.emplace_back(new Thread(i));
    }
}

gearoenix::core::sync::ChanneledWorkers::~ChanneledWorkers() = default;

void gearoenix::core::sync::ChanneledWorkers::perform(const std::function<void(std::uint32_t)>& job)
{
    std::uint32_t min = std::numeric_limits<std::uint32_t>::max();
    std::uint32_t i = 0;
    for (const auto& t : threads) {
        std::uint32_t p = t->pending.size();
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
