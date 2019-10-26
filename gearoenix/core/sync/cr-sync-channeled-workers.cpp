#include "cr-sync-channeled-workers.hpp"

void gearoenix::core::sync::ChanneledWorkers::Thread::kernel() noexcept
{
    c.receive()(i);
    s->release();
}

gearoenix::core::sync::ChanneledWorkers::Thread::Thread(const std::size_t i, Semaphore* const s) noexcept
    : i(i)
    , s(s)
    , t(std::bind(&Thread::kernel, this))
{
}

gearoenix::core::sync::ChanneledWorkers::ChanneledWorkers() noexcept
{
    std::size_t count = std::thread::hardware_concurrency();
    threads.reserve(count);
    for (std::size_t i = 0; i < count; ++i)
        threads.emplace_back(i, &s);
}

void gearoenix::core::sync::ChanneledWorkers::perform(const std::function<void(std::size_t)>& job) noexcept
{
    std::size_t min = std::numeric_limits<std::size_t>::max();
    std::size_t i = 0;
    for (const Thread& t : threads) {
        const std::size_t p = t.c.get_pending();
        if (p < min) {
            min = p;
            i = t.i;
        }
    }
    threads[i].c.send(job);
    jobs_count++;
}

void gearoenix::core::sync::ChanneledWorkers::wait() noexcept
{
    const std::size_t jc = jobs_count.load();
    for (std::size_t i = 0; i < jc; ++i)
        s.lock();
}