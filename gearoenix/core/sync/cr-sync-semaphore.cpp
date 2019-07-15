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
    while (count <= 0)
        c.wait(lock);
    --count;
}

void gearoenix::core::sync::Semaphore::release()
{
    ++count;
    c.notify_one();
}
