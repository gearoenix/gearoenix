#include "cr-sync-queued-semaphore.hpp"
#include "../../system/sys-log.hpp"
#include <thread>

gearoenix::core::sync::QueuedSemaphore::QueuedSemaphore(int count)
    : count(count)
{
}

gearoenix::core::sync::QueuedSemaphore::~QueuedSemaphore() {}

void gearoenix::core::sync::QueuedSemaphore::lock()
{
    GXLOGD("Semaphore count is: " << count);
    m_count.lock();
    GXLOGD("Semaphore count is: " << count);
    if (count > 0) {
        --count;
        m_count.unlock();
        return;
    }
    Lock l(new LockData());
    q.push(l);
    std::unique_lock<std::mutex> lock(l->m);
    m_count.unlock();
    l->c.wait(lock, [&] {
        return l->unlocked;
    });
    l->locked = false;
    lock.unlock();
}

void gearoenix::core::sync::QueuedSemaphore::release()
{
    m_count.lock();
    if (q.size() < 1) {
        ++count;
        m_count.unlock();
        return;
    }
    Lock l = q.front();
    q.pop();
    m_count.unlock();
    do {
        GXLOGD("waiting");
        l->unlocked = true;
        l->c.notify_all();
        std::this_thread::yield();
    } while (l->locked);
}
