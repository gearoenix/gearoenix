#ifndef GEAROENIX_PHYSICS_KERNEL_HPP
#define GEAROENIX_PHYSICS_KERNEL_HPP
#include "../core/cr-build-configuration.hpp"
#include "../core/cr-types.hpp"
#include <chrono>
#ifdef THREAD_SUPPORTED
#include <thread>
#endif
namespace gearoenix {
#ifdef THREAD_SUPPORTED
namespace core {
    class Semaphore;
}
#endif
namespace physics {
    class Engine;
    class Kernel {
    private:
        Engine* engine;
        std::chrono::system_clock::time_point last_update = std::chrono::system_clock::now();
        std::chrono::system_clock::time_point now;
        core::Real delta_time;
#ifdef THREAD_SUPPORTED
        const unsigned int thread_index;
        core::Semaphore* signaller;
        std::thread thread;
        volatile bool alive = true;
#endif
        void run();
        void apply_animations();
        void apply_constraints();

    protected:
    public:
        Kernel(
#ifdef THREAD_SUPPORTED
            const unsigned int thread_index,
#endif
            Engine* engine);
        ~Kernel();
        void signal();
    };
}
}
#endif
