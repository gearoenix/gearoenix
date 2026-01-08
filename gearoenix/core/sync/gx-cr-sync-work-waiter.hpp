#pragma once
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
    std::atomic<State> state = State::Working;

    void wait_loop();

public:
    WorkWaiter();
    ~WorkWaiter();
    void push(std::function<void()>&& f);
};
}