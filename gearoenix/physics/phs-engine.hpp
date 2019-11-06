#ifndef GEAROENIX_PHYSICS_ENGINE_HPP
#define GEAROENIX_PHYSICS_ENGINE_HPP
#include "../core/cr-pool.hpp"
#include "../core/cr-static.hpp"
#include "../core/cr-types.hpp"
#include "../math/math-vector.hpp"
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <tuple>
#include <vector>
namespace gearoenix {
namespace core::sync {
    class KernelWorkers;
}
namespace render {
    namespace camera {
        class Camera;
    }
    namespace engine {
        class Engine;
    }
    namespace light {
        class Directional;
        class CascadeInfo;
        class Light;
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
        struct Manager;
    }
    class Engine {
        GX_GET_UCPTR_PRV(animation::Manager, animation_manager)
    public:
        struct PooledShadowCasterDirectionalLights {
            render::light::Directional* light = nullptr;
            std::unique_ptr<render::light::CascadeInfo> cascades_info;
            explicit PooledShadowCasterDirectionalLights(render::engine::Engine* eng) noexcept;
            ~PooledShadowCasterDirectionalLights() noexcept;
        };

        struct PooledCameraData {
            render::camera::Camera* camera = nullptr;
            std::vector<render::model::Model*> opaque_container_models;
            std::map<core::Real, render::model::Model*> transparent_container_models;
            core::OneLoopPool<PooledShadowCasterDirectionalLights> shadow_caster_directional_lights;
        };

        struct PooledSceneData {
            render::scene::Scene* scene = nullptr;
            core::OneLoopPool<PooledCameraData> cameras;
        };

        struct GatheredCameraData {
            std::vector<render::model::Model*> opaque_container_models;
            std::map<core::Real, render::model::Model*> transparent_container_models;
            std::map<core::Real, std::map<render::light::Directional*, render::light::CascadeInfo*>> shadow_caster_directional_lights;
        };

        struct GatheredSceneData {
            std::map<core::Real, std::map<render::camera::Camera*, GatheredCameraData>> priority_ptr_camera;
        };

        struct GatheredData {
            std::map<core::Real, std::map<render::scene::Scene*, GatheredSceneData>> priority_ptr_scene;
        };

    private:
        system::Application* const sys_app;
        core::sync::KernelWorkers* const workers;
        /// visibility checker
        std::vector<core::OneLoopPool<PooledSceneData>> kernels_scene_camera_data;
        GatheredData scenes_camera_data;

        /// It does:
        ///    - Updates scenes (uniform buffer, dynamic bvh trees, static bvh tree (if was needed))
        void update_scenes_kernel(unsigned int kernel_index) noexcept;
        void update_scenes_receiver() noexcept;

        /// It does followings:
        ///    - Uniform buffer update for scene.
        ///    - Uniform buffer update for camera.
        ///    - It does the model visibility check.
        ///    - Uniform buffer update for model.
        ///    - Update the shadow caster lights.
        ///    - Compute cameras cascaded shadowing partitions
        void update_visibility_kernel(unsigned int kernel_index) noexcept;
        void update_visibility_receiver() noexcept;

        /// It does followings:
        ///    - Directional shadow caster lights cascade kernels gathering
        void update_shadower_kernel(unsigned int kernel_index) noexcept;
        void update_shadower_receiver() noexcept;

    public:
        Engine(system::Application* sysapp, core::sync::KernelWorkers* workers) noexcept;
        ~Engine() noexcept;
        void update() noexcept;
        [[nodiscard]] const GatheredData& get_visible_models() const noexcept;
        [[nodiscard]] GatheredData& get_visible_models() noexcept;
    };
}
}
#endif
