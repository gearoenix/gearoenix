#ifndef GEAROENIX_PHYSICS_ENGINE_HPP
#define GEAROENIX_PHYSICS_ENGINE_HPP
#include "../core/gx-cr-static.hpp"
#include <map>

namespace gearoenix::core::sync {
struct KernelWorkers;
}

namespace gearoenix::platform {
struct Application;
}

namespace gearoenix::physics::animation {
struct Manager;
}

namespace gearoenix::render::scene {
struct Scene;
}

namespace gearoenix::physics {
struct Engine {
    typedef std::multimap<double, std::shared_ptr<render::scene::Scene>> SceneMap;
    GX_GET_UCPTR_PRV(animation::Manager, animation_manager)
    GX_GET_CREF_PRV(SceneMap, sorted_scenes)
private:
    platform::Application* const platform_application;
    core::sync::KernelWorkers* const workers;

    /// It does:
    ///    - Updates scenes (uniform buffer, dynamic bvh trees, static bvh tree (if was needed))
    ///    - Update the shadow caster lights.
    ///    - Compute cameras cascaded shadowing partitions
    void update_scenes_kernel(unsigned int kernel_index) noexcept;
    void update_scenes_receiver() noexcept;

    /// It does followings:
    ///    - Uniform buffer update for scene.
    ///    - Uniform buffer update for camera.
    ///    - It does the model visibility check.
    ///    - Uniform buffer update for model.
    ///    - Directional shadow caster lights cascade kernels gathering
    void update_visibility_kernel(unsigned int kernel_index) noexcept;
    void update_visibility_receiver() noexcept;

public:
    Engine(platform::Application* platform_application, core::sync::KernelWorkers* workers) noexcept;
    ~Engine() noexcept;
    void update() noexcept;
};
}
#endif
