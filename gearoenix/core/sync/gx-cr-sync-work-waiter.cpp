#include "gx-cr-sync-work-waiter.hpp"
#include "../macro/gx-cr-mcr-assert.hpp"
#include <utility>

void gearoenix::core::sync::WorkWaiter::wait_loop()
{
    state = State::Working;
    while (State::Working == state) {
        semaphore.lock();
        if (State::Working != state)
            break;
        function_loader.unload();
    }
    state = State::Terminated;
    GX_LOG_D("Worker thread: " << std::this_thread::get_id() << " is finished.");
}

gearoenix::core::sync::WorkWaiter::WorkWaiter()
    : thread([this] { wait_loop(); })
{
}

gearoenix::core::sync::WorkWaiter::~WorkWaiter()
{
    GX_CHECK_NOT_EQUAL_D(state, State::Terminated);
    do {
        state = State::Finished;
        semaphore.release();
    } while (State::Terminated != state);
    thread.join();
}

void gearoenix::core::sync::WorkWaiter::push(std::function<void()>&& f)
{
    function_loader.load(std::move(f));
    semaphore.release();
}