#include "cr-sync-semaphore.hpp"
#include <thread>
#include "../../system/sys-log.hpp"

gearoenix::core::sync::Semaphore::Semaphore(int count)
    : count(count)
{
}

gearoenix::core::sync::Semaphore::~Semaphore() {}

void gearoenix::core::sync::Semaphore::lock()
{
    m_count.lock();
    if (count > 0) {
        --count;
        m_count.unlock();
        return;
    }
    Lock l(new LockData);
    q.push(l);
    m_count.unlock();
    std::unique_lock<std::mutex> lock(l->m);
    l->c.wait(lock, [l] {
        return l->unlocked;
    });
    l->locked = false;
    lock.unlock();
}

void gearoenix::core::sync::Semaphore::release()
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
        {
        std::lock_guard<std::mutex> _l(l->m);
        l->unlocked = true;
        }
        l->c.notify_all();
        std::this_thread::yield();
    } while (l->locked);
}
