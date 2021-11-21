#include "gx-cr-sync-work-waiter.hpp"

#ifdef GX_THREAD_NOT_SUPPORTED

gearoenix::core::sync::WorkWaiter::WorkWaiter() = default;
gearoenix::core::sync::WorkWaiter::~WorkWaiter() = default;

void gearoenix::core::sync::WorkWaiter::push(const std::function<void()>& f)
{
    f();
    (void)this;
}

#else

#include "../macro/gx-cr-mcr-assert.hpp"
#include <utility>

void gearoenix::core::sync::WorkWaiter::wait_loop() noexcept
{
    state = State::Working;
    while (State::Working == state) {
        semaphore.lock();
        if (State::Working != state)
            break;
        function_loader.unload();
    }
    state = State::Terminated;
    GX_LOG_D("Worker thread: " << std::this_thread::get_id() << " is finished.")
}

gearoenix::core::sync::WorkWaiter::WorkWaiter() noexcept
    : thread([this] { wait_loop(); })
{
}

gearoenix::core::sync::WorkWaiter::~WorkWaiter() noexcept
{
    GX_CHECK_NOT_EQUAL_D(state, State::Terminated)
    do {
        state = State::Finished;
        semaphore.release();
    } while (State::Terminated != state);
    thread.join();
}

void gearoenix::core::sync::WorkWaiter::push(std::function<void()>&& f) noexcept
{
    function_loader.load(std::move(f));
    semaphore.release();
}

#endif