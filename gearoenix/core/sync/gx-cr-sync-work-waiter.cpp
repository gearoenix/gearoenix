#include "gx-cr-sync-work-waiter.hpp"
#include "../macro/gx-cr-mcr-assert.hpp"
#include <utility>

void gearoenix::core::sync::WorkWaiter::wait_loop()
{
    state = State::Working;
    while (State::Working == state.load(std::memory_order_relaxed)) {
        semaphore.lock();
        if (State::Working != state.load(std::memory_order_relaxed)) {
            break;
        }
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
    while (State::Terminated != state.exchange(State::Finished, std::memory_order_relaxed)) {
        semaphore.release();
    }
    thread.join();
}

void gearoenix::core::sync::WorkWaiter::push(std::function<void()>&& f)
{
    function_loader.load(std::move(f));
    semaphore.release();
}