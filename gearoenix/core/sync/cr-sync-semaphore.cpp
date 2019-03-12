#include "cr-sync-semaphore.hpp"

gearoenix::core::sync::Semaphore::Semaphore(unsigned int c)
    : c(c)
{}

void gearoenix::core::sync::Semaphore::signal()
{
    std::unique_lock<std::mutex> lock(m);
    ++c;
    cv.notify_one();
}

void gearoenix::core::sync::Semaphore::wait()
{
    std::unique_lock<std::mutex> lock(m);
    cv.wait(lock, [&]{ return c > 0; });
    --c;
}
