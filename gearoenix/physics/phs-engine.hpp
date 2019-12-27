#ifndef GEAROENIX_PHYSICS_ENGINE_HPP
#define GEAROENIX_PHYSICS_ENGINE_HPP
#include "../core/cr-pool.hpp"
#include "../core/cr-static.hpp"
#include "../core/cr-types.hpp"
#include "../math/math-vector.hpp"
#include "../render/material/rnd-mat-type.hpp"
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <tuple>
#include <vector>

namespace gearoenix::core::sync {
class KernelWorkers;
}

namespace gearoenix::render::camera {
class Camera;
}

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::render::light {
class Directional;
class CascadeInfo;
class Light;
}

namespace gearoenix::render::mesh {
class Mesh;
}

namespace gearoenix::render::model {
class Model;
}

namespace gearoenix::render::scene {
class Scene;
}

namespace gearoenix::system {
class Application;
}

namespace gearoenix::physics::animation {
struct Manager;
}

namespace gearoenix::physics {
class Engine {
    GX_GET_UCPTR_PRV(animation::Manager, animation_manager)
public:
    struct PooledShadowCasterDirectionalLights {
        render::light::Directional* light = nullptr;
        std::unique_ptr<render::light::CascadeInfo> cascades_info;
        explicit PooledShadowCasterDirectionalLights(render::engine::Engine* eng) noexcept;
        ~PooledShadowCasterDirectionalLights() noexcept;
    };

    using OpaqueMeshes = std::map<render::material::Type, std::map<render::model::Model*, render::mesh::Mesh*>>;
    using TransparentMeshes = std::map<core::Real, std::map<render::material::Type, std::map<render::model::Model*, render::mesh::Mesh*>>>;

    struct PooledCameraData {
        render::camera::Camera* camera = nullptr;
        OpaqueMeshes opaque_container_models;
        TransparentMeshes transparent_container_models;
        core::OneLoopPool<PooledShadowCasterDirectionalLights> shadow_caster_directional_lights;
    };

    struct PooledSceneData {
        render::scene::Scene* scene = nullptr;
        core::OneLoopPool<PooledCameraData> cameras;
    };

    struct GatheredCameraData {
        OpaqueMeshes opaque_container_models;
        TransparentMeshes transparent_container_models;
        std::set<std::tuple<core::Real, render::light::Directional*, render::light::CascadeInfo*>> shadow_caster_directional_lights;
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
    GX_GET_CREF_PRV(GatheredData, scenes_camera_data)

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
    Engine(system::Application* sys_app, core::sync::KernelWorkers* workers) noexcept;
    ~Engine() noexcept;
    void update() noexcept;
};
}
#endif
