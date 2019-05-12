#ifndef GEAROENIX_PHYSICS_ENGINE_HPP
#define GEAROENIX_PHYSICS_ENGINE_HPP
#include "../core/cr-types.hpp"
#include <map>
#include <memory>
#include <mutex>
#include <vector>
namespace gearoenix {
namespace core {
    namespace sync {
        class KernelWorker;
    }
}
namespace system {
    class Application;
}
namespace physics {
    namespace animation {
        class Animation;
    }
    class Engine {
    private:
        const std::shared_ptr<system::Application> sysapp;
        const std::shared_ptr<core::sync::KernelWorker> kernels;
        // if animation return true on its apply its gonna be deleted
        std::map<core::Id, std::shared_ptr<animation::Animation>> animations;
        std::mutex added_animations_locker;
        std::vector<std::shared_ptr<animation::Animation>> added_animations;
        std::mutex removed_animations_locker;
        std::vector<core::Id> removed_animations;

        void update_uniform_buffers_kernel(const unsigned int kernel_index);
        void update_uniform_buffers();

    public:
        Engine(const std::shared_ptr<system::Application>& sysapp, const std::shared_ptr<core::sync::KernelWorker> &kernels);
        ~Engine();
        void add_animation(const std::shared_ptr<animation::Animation>& a);
        void remove_animation(const std::shared_ptr<animation::Animation>& a);
        void remove_animation(core::Id a);
        void update();
    };
}
}
#endif
