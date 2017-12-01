#ifndef GEAROENIX_PHYSICS_KERNEL_HPP
#define GEAROENIX_PHYSICS_KERNEL_HPP
#include <thread>
#include <tuple>
#include <vector>
namespace gearoenix {
namespace core {
    class Semaphore;
}
namespace physics {
    class Engine;
    class Kernel {
    private:
        const unsigned int thread_index;
        Engine* engine;
        core::Semaphore* signaller;
        std::thread thread;
        volatile bool alive = true;
        void run();

    protected:
    public:
        Kernel(const unsigned int thread_index, Engine* engine);
        ~Kernel();
        void signal();
    };
}
}
#endif
