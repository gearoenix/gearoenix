#include "cr-sync-work-waiter.hpp"
#include "../../system/sys-log.hpp"
#include "../cr-function-loader.hpp"
#include "cr-sync-semaphore.hpp"

#include <iostream>

void gearoenix::core::sync::WorkWaiter::wait_loop()
{

    while (state == WORKING) {
        GXLOGD("Going to wait.");
        semaphore->lock();
        GXLOGD("Going to unload.");
        function_loader->unload();
    }
    state = TERMINATED;
}

gearoenix::core::sync::WorkWaiter::WorkWaiter()
    : semaphore(new Semaphore())
    , function_loader(new FunctionLoader())
    , thread(std::bind(&WorkWaiter::wait_loop, this))
{
}

gearoenix::core::sync::WorkWaiter::~WorkWaiter()
{
#ifdef GX_DEBUG_MODE
    if (state == TERMINATED)
        GXLOGF("Error terminated sooner than expected.");
#endif
    state = FINISHED;
    while (state != TERMINATED) {
        semaphore->release();
    }
    thread.join();
}

void gearoenix::core::sync::WorkWaiter::push(std::function<void()> f)
{
    function_loader->load(f);
    semaphore->release();
}
