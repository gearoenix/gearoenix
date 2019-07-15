#include "cr-sync-semaphore.hpp"
#include "../../system/sys-log.hpp"
#include <thread>

gearoenix::core::sync::Semaphore::Semaphore(int count)
    : count(count)
{
}

gearoenix::core::sync::Semaphore::~Semaphore() {}

void gearoenix::core::sync::Semaphore::lock()
{
    std::unique_lock<std::mutex> lock(m);
    c.wait(lock, [this] { return count > 0; });
    --count;
}

void gearoenix::core::sync::Semaphore::release()
{
    std::lock_guard<std::mutex> _lock(m);
    ++count;
    c.notify_one();
}
