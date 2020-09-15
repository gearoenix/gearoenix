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
#include "../../system/gx-sys-log.hpp"
#include "../gx-cr-function-loader.hpp"
#include "gx-cr-sync-semaphore.hpp"
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
    , thread([this] { wait_loop(); })
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

void gearoenix::core::sync::WorkWaiter::push(const std::function<void()>& f)
{
    function_loader->load(f);
    semaphore->release();
}
#endif