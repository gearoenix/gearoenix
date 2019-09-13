#include "cr-sync-work-waiter.hpp"
#include "../../system/sys-log.hpp"
#include "../cr-function-loader.hpp"
#include "cr-sync-semaphore.hpp"

#include <iostream>
#include <utility>

void gearoenix::core::sync::WorkWaiter::wait_loop()
{
    main_thread_state.store(State::Working);
    while (main_thread_state.load() == State::Working) {
        semaphore->lock();
        if (main_thread_state.load() != State::Working)
            break;
        function_loader->unload();
    }
    worker_thread_state.store(State::Terminated);
    GXLOGD("Worker thread " << reinterpret_cast<std::size_t>(this) << " is finished.")
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
    if (worker_thread_state.load() == State::Terminated)
        GXLOGF("Error terminated sooner than expected.")
#endif
    do {
        main_thread_state.store(State::Finished);
        semaphore->release();
    } while (worker_thread_state.load() != State::Terminated);
    thread.join();
    main_thread_state = State::Terminated;
}

void gearoenix::core::sync::WorkWaiter::push(std::function<void()> f)
{
    function_loader->load(std::move(f));
    semaphore->release();
}
