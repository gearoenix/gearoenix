#include "test-006-stop-point.hpp"
#ifdef TEST006
#include "../core/sync/cr-sync-stop-point.hpp"
#include "../system/sys-log.hpp"
#include <chrono>
#include <thread>

#ifndef IN_WINDOWS
int main()
#else
#include <Windows.h>
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif
{
    gearoenix::core::sync::StopPoint sp(2);
    auto start = std::chrono::system_clock::now();
    std::thread t1([&] {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        GXLOGI("1 reached." << std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::system_clock::now() - start).count());
        sp.all_reach();
        GXLOGI("1 reached." << std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::system_clock::now() - start).count());
        std::this_thread::sleep_for(std::chrono::seconds(1));
        GXLOGI("1 reached." << std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::system_clock::now() - start).count());
        sp.all_reach();
    });
    std::thread t2([&] {
        std::this_thread::sleep_for(std::chrono::seconds(9));
        GXLOGI("2 reached." << std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::system_clock::now() - start).count());
        sp.all_reach();
        GXLOGI("2 reached." << std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::system_clock::now() - start).count());
        std::this_thread::sleep_for(std::chrono::seconds(9));
        GXLOGI("2 reached." << std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::system_clock::now() - start).count());
        sp.all_reach();
    });
    t1.join();
    t2.join();
    return 0;
}

#endif
