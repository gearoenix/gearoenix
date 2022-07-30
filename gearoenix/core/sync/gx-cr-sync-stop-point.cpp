#include "gx-cr-sync-stop-point.hpp"
#include "gx-cr-sync-semaphore.hpp"

gearoenix::core::sync::StopPoint::StopPoint(int walkers_count)
    : sem(new Semaphore())
    , stopper(walkers_count)
{
}

void gearoenix::core::sync::StopPoint::all_reach()
{
    counter_lock.lock();
    counter++;
    if (counter == stopper) {
        counter = 0;
        for (int i = 1; i < stopper; ++i)
            sem->release();
        counter_lock.unlock();
        return;
    } else {
        counter_lock.unlock();
        sem->lock();
    }
}