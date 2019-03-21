#include "../cr-build-configuration.hpp"
#include "cr-sync-semaphore.hpp"
#include "../../system/sys-log.hpp"
#include <thread>

void gearoenix::core::sync::Signaler::signal()
{
	do {
		unlocked = true;
		cv.notify_one();
		std::this_thread::yield();
	} while (locked);
}

void gearoenix::core::sync::Signaler::wait()
{
	locked = true;
	unlocked = false;
    std::unique_lock<std::mutex> lock(m);
    cv.wait(lock, [&]{ return unlocked; });
	locked = false;
	unlocked = true;
}
