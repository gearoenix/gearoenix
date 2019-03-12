#include "test-005-semaphore.hpp"
#ifdef GX_TEST005
#include "../core/sync/cr-sync-semaphore.hpp"
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
    gearoenix::core::sync::Semaphore sem;
    std::thread t1([&] {
        sem.release();
    });
    std::thread t2([&] {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        GXLOGI("Waiting for signal...");
        sem.lock();
        GXLOGI("Signal recieved.");
    });
    t1.join();
    t2.join();
    return 0;
}

#endif
