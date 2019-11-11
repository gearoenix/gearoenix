#include "cr-sync-semaphore.hpp"

gearoenix::core::sync::Semaphore::Semaphore(int count) noexcept
    : count(count)
{
}

void gearoenix::core::sync::Semaphore::lock() noexcept
{
    std::unique_lock<std::mutex> lock(m);
    c.wait(lock, [this] {
        return count > 0;
    });
    --count;
}

void gearoenix::core::sync::Semaphore::release() noexcept
{
    std::lock_guard<std::mutex> _lock(m);
    ++count;
    c.notify_all();
}
