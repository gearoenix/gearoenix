#ifdef GX_TEST005
#include "../core/sync/cr-sync-semaphore.hpp"
#include "../system/sys-log.hpp"
#include <chrono>
#include <thread>

int main()
{
    gearoenix::core::sync::Semaphore sem;
    constexpr auto loop_count = 10000000;
    std::thread t0([&] {
        for (int i = 0; i < loop_count; ++i)
            sem.lock();
    });
    std::thread t1([&] {
        for (int i = 0; i < loop_count; ++i)
            sem.release();
    });
    std::thread t2([&] {
        for (int i = 0; i < loop_count; ++i)
            sem.lock();
    });
    std::thread t3([&] {
        for (int i = 0; i < loop_count; ++i)
            sem.release();
    });
    std::thread t4([&] {
        for (int i = 0; i < loop_count; ++i)
            sem.lock();
    });
    std::thread t5([&] {
        for (int i = 0; i < loop_count; ++i)
            sem.release();
    });
    std::thread t6([&] {
        for (int i = 0; i < loop_count; ++i)
            sem.lock();
    });
    std::thread t7([&] {
        for (int i = 0; i < loop_count; ++i)
            sem.release();
    });
    t0.join();
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    for (int i = 0; i < loop_count; ++i)
        sem.release();
    for (int i = 0; i < loop_count; ++i)
        sem.lock();
    return 0;
}

#endif
