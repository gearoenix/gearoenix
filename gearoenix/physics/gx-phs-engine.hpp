#pragma once
#include "../core/gx-cr-singleton.hpp"
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
struct Engine final : core::Singleton<Engine> {
    GX_GET_UCPTR_PRV(animation::Manager, animation_manager);
    GX_GET_UCPTR_PRV(constraint::Manager, constraint_manager);

public:
    Engine();
    ~Engine() override;
    void start_frame();
    void end_frame();
};
}