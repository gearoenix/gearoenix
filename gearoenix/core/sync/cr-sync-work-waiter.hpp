#ifndef GEAROENIX_CORE_SYNC_STOP_POINT_HPP
#define GEAROENIX_CORE_SYNC_STOP_POINT_HPP
#include <atomic>
#include <functional>
#include <memory>
#include <thread>

namespace gearoenix::core {
class FunctionLoader;
namespace sync {
    class Semaphore;
    class WorkWaiter {
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

    public:
        WorkWaiter();
        ~WorkWaiter();
        void push(std::function<void()> f);
    };
}
}
#endif
