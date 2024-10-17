#ifndef GEAROENIX_PHYSICS_ENGINE_HPP
#define GEAROENIX_PHYSICS_ENGINE_HPP
#include "../core/macro/gx-cr-mcr-getter-setter.hpp"
#include <memory>

namespace gearoenix::physics::animation {
struct Manager;
}

namespace gearoenix::physics::constraint {
struct Manager;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::physics {
struct Engine final {
    GX_GET_RRF_PRV(render::engine::Engine, render_engine);
    GX_GET_UCPTR_PRV(animation::Manager, animation_manager);
    GX_GET_UCPTR_PRV(constraint::Manager, constraint_manager);

public:
    explicit Engine(render::engine::Engine& render_engine);
    ~Engine();
    Engine(Engine&&) = delete;
    Engine(const Engine&) = delete;
    void start_frame();
    void end_frame();
};
}
#endif
