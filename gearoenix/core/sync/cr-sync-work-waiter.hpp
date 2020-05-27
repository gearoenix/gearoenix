#ifndef GEAROENIX_CORE_SYNC_WORK_WAITER_HPP
#define GEAROENIX_CORE_SYNC_WORK_WAITER_HPP
#include "../cr-build-configuration.hpp"
#include <functional>

#ifndef GX_THREAD_NOT_SUPPORTED
#include <atomic>
#include <memory>
#include <thread>
#endif

namespace gearoenix::core {
class FunctionLoader;
}

namespace gearoenix::core::sync {
#ifndef GX_THREAD_NOT_SUPPORTED
class Semaphore;
#endif
class WorkWaiter {
#ifndef GX_THREAD_NOT_SUPPORTED
private:
    enum struct State : int {
        Working,
        Finished,
        Terminated,
    };

    const std::shared_ptr<Semaphore> semaphore;
    const std::shared_ptr<FunctionLoader> function_loader;
    std::thread thread;
    std::atomic<State> main_thread_state = State::Working;
    std::atomic<State> worker_thread_state = State::Working;

    void wait_loop();

#endif
public:
    WorkWaiter();
    ~WorkWaiter();
    void push(const std::function<void()>& f);
};
}

#endif