#ifndef GEAROENIX_CORE_SYNC_WORK_WAITER_HPP
#define GEAROENIX_CORE_SYNC_WORK_WAITER_HPP
#include "../gx-cr-build-configuration.hpp"
#include "../gx-cr-function-loader.hpp"
#include "gx-cr-sync-semaphore.hpp"
#include <functional>
#include <thread>

namespace gearoenix::core {
struct FunctionLoader;
}

namespace gearoenix::core::sync {
struct WorkWaiter final {
private:
    enum struct State {
        Working,
        Finished,
        Terminated,
    };

    Semaphore semaphore;
    FunctionLoader function_loader;
    std::thread thread;
    State state = State::Working;

    void wait_loop() noexcept;

public:
    WorkWaiter() noexcept;
    ~WorkWaiter() noexcept;
    void push(std::function<void()>&& f) noexcept;
};
}

#endif