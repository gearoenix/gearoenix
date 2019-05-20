#ifndef GEAROENIX_PHYSICS_ENGINE_HPP
#define GEAROENIX_PHYSICS_ENGINE_HPP
#include "../core/cr-types.hpp"
#include "../math/math-vector.hpp"
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
    namespace light {
        class Directional;
        class CascadeInfo;
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
    public:
        using ModelSet = std::set<std::shared_ptr<render::model::Model>>;
        using CameraModelSet = std::map<std::shared_ptr<render::camera::Camera>, ModelSet>;
        using SceneCameraModelSet = std::map<std::shared_ptr<render::scene::Scene>, CameraModelSet>;
        using Partition = math::Vec3[4];
        using Partitions = std::vector<Partition>;
        using CameraPartitions = std::map<std::shared_ptr<render::camera::Camera>, Partitions>;
        using SceneCameraPartitions = std::map<std::shared_ptr<render::scene::Scene>, CameraPartitions>;
        using LightCascadeInfo = std::map<std::shared_ptr<render::light::Directional>, std::shared_ptr<render::light::CascadeInfo>>;
        using CameraLightCascadeInfo = std::map<std::shared_ptr<render::camera::Camera>, LightCascadeInfo>;
        using SceneCameraLightCascadeInfo = std::map<std::shared_ptr<render::scene::Scene>, CameraLightCascadeInfo>;
    private:
        const std::shared_ptr<system::Application> sysapp;
        const std::shared_ptr<core::sync::KernelWorker> kernels;
        // if animation return true on its apply its gonna be deleted
        std::map<core::Id, std::shared_ptr<animation::Animation>> animations;
        std::vector<SceneCameraModelSet> kernels_visible_models;
        SceneCameraModelSet visible_models;
        std::vector<SceneCameraPartitions> kernels_cascaded_shadows_partitions;
        SceneCameraPartitions cascaded_shadows_partitions;
        std::vector<SceneCameraLightCascadeInfo> kernels_cascaded_shadow_caster_data;
        SceneCameraLightCascadeInfo cascaded_shadow_caster_data;
        std::mutex added_animations_locker;
        std::vector<std::shared_ptr<animation::Animation>> added_animations;
        std::mutex removed_animations_locker;
        std::vector<core::Id> removed_animations;

        /// It does followings:
        ///    - Uniform buffer update for scene.
        ///    - Uniform buffer update for camera.
        ///    - It does the model visibility check.
        ///    - Uniform buffer update for model.
        ///    - Update the shadow caster lights.
        ///    - Compute cameras cascaded shadowing partitions
        void update_001_kernel(unsigned int kernel_index) noexcept;
        void update_001_receiver() noexcept;

        /// It does followings:
        ///    - Directional shadow caster lights cascade kernels gathering
        void update_002_kernel(unsigned int kernel_index) noexcept;
        void update_002_receiver() noexcept;

    public:
        Engine(std::shared_ptr<system::Application> sysapp, std::shared_ptr<core::sync::KernelWorker> kernels) noexcept;
        void add_animation(const std::shared_ptr<animation::Animation>& a) noexcept;
        void remove_animation(const std::shared_ptr<animation::Animation>& a) noexcept;
        void remove_animation(core::Id a) noexcept;
        void update() noexcept;
        const SceneCameraModelSet& get_visible_models() const noexcept;
        SceneCameraModelSet& get_visible_models() noexcept;
    };
}
}
#endif
