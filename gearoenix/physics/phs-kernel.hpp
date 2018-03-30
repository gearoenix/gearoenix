#ifndef GEAROENIX_PHYSICS_KERNEL_HPP
#define GEAROENIX_PHYSICS_KERNEL_HPP
#include "../core/cr-build-configuration.hpp"
#include "../core/cr-types.hpp"
#include <chrono>
#include <thread>
namespace gearoenix {
namespace core {
    namespace sync {
        class Semaphore;
    }
}
namespace physics {
    class Engine;
    class Kernel {
    private:
        Engine* engine;
        std::chrono::system_clock::time_point last_update = std::chrono::system_clock::now();
        std::chrono::system_clock::time_point now;
        core::Real delta_time;
        const unsigned int thread_index;
        core::sync::Semaphore* signaller;
        std::thread thread;
        volatile bool alive = true;
        void run();
		void update();
        void apply_animations();
        void apply_constraints();
		void apply_bodies();
		void apply_models();

    protected:
    public:
        Kernel(
            const unsigned int thread_index,
            Engine* engine);
        ~Kernel();
        void signal();
    };
}
}
#endif
