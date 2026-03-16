#include "gx-cr-sync-semaphore.hpp"

gearoenix::core::sync::Semaphore::Semaphore(const int count)
    : count(count)
{
}

void gearoenix::core::sync::Semaphore::lock()
{
    std::unique_lock lock(m);
    c.wait(lock, [this] { return count > 0; });
    --count;
}

void gearoenix::core::sync::Semaphore::release()
{
    const std::lock_guard _(m);
    ++count;
    c.notify_one();
}