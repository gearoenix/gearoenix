#include "cr-sync-stop-point.hpp"
#ifndef GX_THREAD_NOT_SUPPORTED
#include "cr-sync-semaphore.hpp"

gearoenix::core::sync::StopPoint::StopPoint(int walkers_count)
    : sem(new Semaphore())
    , stoper(walkers_count)
{
}

void gearoenix::core::sync::StopPoint::all_reach()
{
    counter_lock.lock();
    counter++;
    if (counter == stoper) {
        counter = 0;
        for (int i = 1; i < stoper; ++i)
            sem->release();
        counter_lock.unlock();
        return;
    } else {
        counter_lock.unlock();
        sem->lock();
    }
}
#endif