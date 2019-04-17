#include "test-005-semaphore.hpp"
#ifdef GX_TEST005
#include "../core/sync/cr-sync-queued-semaphore.hpp"
#include "../system/sys-log.hpp"
#include <chrono>
#include <thread>

#ifndef GX_IN_WINDOWS
int main()
#else
#include <Windows.h>
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif
{
    gearoenix::core::sync::QueuedSemaphore sem;
    std::thread t1([&] {
        for (int i = 0; i < 100000; ++i) sem.release();
    });
    std::thread t2([&] {
        for (int i = 0; i < 100000; ++i) sem.lock();
    });
    std::thread t3([&] {
        for (int i = 0; i < 100000; ++i) sem.release();
    });
    std::thread t4([&] {
        for (int i = 0; i < 100000; ++i) sem.lock();
    });
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    for (int i = 0; i < 100; ++i) sem.release();
    for (int i = 0; i < 100; ++i) sem.lock();
    return 0;
}

#endif
