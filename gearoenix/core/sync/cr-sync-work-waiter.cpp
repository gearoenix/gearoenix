#include "cr-sync-work-waiter.hpp"
#include "cr-sync-semaphore.hpp"
#include "../cr-function-loader.hpp"

void gearoenix::core::sync::WorkWaiter::wait_loop()
{
	while (running) {
		semaphore->lock();
		function_loader->unload_all();
	}
	running = true;
}

gearoenix::core::sync::WorkWaiter::WorkWaiter()
	: semaphore(new Semaphore())
	, function_loader(new FunctionLoader())
	, thread(std::bind(&WorkWaiter::wait_loop, this))
{
}

gearoenix::core::sync::WorkWaiter::~WorkWaiter()
{
	running = false;
	while (!running) {
		semaphore->release();
	}
	thread.join();
}

void gearoenix::core::sync::WorkWaiter::push(std::function<void()> f)
{
	function_loader->load(f);
	semaphore->release();
}
