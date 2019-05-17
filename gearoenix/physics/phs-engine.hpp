#ifndef GEAROENIX_PHYSICS_ENGINE_HPP
#define GEAROENIX_PHYSICS_ENGINE_HPP
#include "../core/cr-types.hpp"
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <vector>
namespace gearoenix {
namespace core::sync {
    class KernelWorker;
}
namespace render {
    namespace camera {
        class Camera;
    }
    namespace model {
        class Model;
    }
    namespace scene {
        class Scene;
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
        std::vector<std::map<std::shared_ptr<render::scene::Scene>, std::map<std::shared_ptr<render::camera::Camera>, std::set<std::shared_ptr<render::model::Model>>>>> kernels_visible_models;
        std::map<std::shared_ptr<render::scene::Scene>, std::map<std::shared_ptr<render::camera::Camera>, std::set<std::shared_ptr<render::model::Model>>>> visible_models;
        std::mutex added_animations_locker;
        std::vector<std::shared_ptr<animation::Animation>> added_animations;
        std::mutex removed_animations_locker;
        std::vector<core::Id> removed_animations;

        void update_uniform_buffers_kernel(unsigned int kernel_index);
        void update_uniform_buffers();

    public:
        Engine(std::shared_ptr<system::Application> sysapp, std::shared_ptr<core::sync::KernelWorker> kernels);
        void add_animation(const std::shared_ptr<animation::Animation>& a);
        void remove_animation(const std::shared_ptr<animation::Animation>& a);
        void remove_animation(core::Id a);
        void update();
        const std::map<std::shared_ptr<render::scene::Scene>, std::map<std::shared_ptr<render::camera::Camera>, std::set<std::shared_ptr<render::model::Model>>>>& get_visible_models() const noexcept;
        std::map<std::shared_ptr<render::scene::Scene>, std::map<std::shared_ptr<render::camera::Camera>, std::set<std::shared_ptr<render::model::Model>>>>& get_visible_models() noexcept;
    };
}
}
#endif
