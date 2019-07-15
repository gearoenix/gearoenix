#ifndef GEAROENIX_CORE_SYNC_STOP_POINT_HPP
#define GEAROENIX_CORE_SYNC_STOP_POINT_HPP
#include <functional>
#include <memory>
#include <thread>

namespace gearoenix::core {
class FunctionLoader;
namespace sync {
    class Semaphore;
    class WorkWaiter {
    private:
        typedef enum {
            WORKING,
            FINISHED,
            TERMINATED,
        } work_state;

        const std::shared_ptr<Semaphore> semaphore;
        const std::shared_ptr<FunctionLoader> function_loader;
        std::thread thread;
        work_state state = WORKING;

        void wait_loop();

    public:
        WorkWaiter();
        ~WorkWaiter();
        void push(std::function<void()> f);
    };
}
}
#endif
