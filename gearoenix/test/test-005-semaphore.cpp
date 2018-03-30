#include "test-005-semaphore.hpp"
#ifdef TEST005
#include "../system/sys-log.hpp"
#include "../core/sync/cr-sync-semaphore.hpp"
#include <thread>
#include <chrono>

#ifndef IN_WINDOWS
int main()
#else
#include <Windows.h>
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif
{
	gearoenix::core::sync::Semaphore sem;
	std::thread t1([&] {
		std::this_thread::sleep_for(std::chrono::seconds(5));
		sem.release();
	});
	std::thread t2([&] {
		GXLOGI("Waiting for signal...");
		sem.lock();
		GXLOGI("Signal recieved.");
	});
	t1.join();
	t2.join();
    return 0;
}

#endif
