#pragma once
#include "../core/gx-cr-singleton.hpp"
#include "../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-au-manager.hpp"

namespace FMOD {
class System;
}

namespace gearoenix::platform {
struct Application;
}

namespace gearoenix::audio {
struct Engine final : core::Singleton<Engine> {
    GX_GET_REF_PRV(Manager, manager);
    GX_GET_PTR_PRV(FMOD::System, system);

public:
    Engine();
    ~Engine() override;

    void update();
};
}