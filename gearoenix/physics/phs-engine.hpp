#ifndef GEAROENIX_PHYSICS_ENGINE_HPP
#define GEAROENIX_PHYSICS_ENGINE_HPP
#include "../core/cr-static.hpp"

namespace gearoenix::core::sync {
class KernelWorkers;
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
private:
    system::Application* const sys_app;
    core::sync::KernelWorkers* const workers;

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
