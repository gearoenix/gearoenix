#ifndef GEAROENIX_AUDIO_ENGINE_HPP
#define GEAROENIX_AUDIO_ENGINE_HPP
#include "../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../core/sync/gx-cr-sync-semaphore.hpp"
#include "../platform/gx-plt-build-configuration.hpp"
#include "gx-au-manager.hpp"
#include <chrono>
#include <thread>

namespace FMOD {
class System;
}

namespace gearoenix::platform {
struct Application;
}

namespace gearoenix::audio {
struct Engine final {
    GX_GET_RRF_PRV(platform::Application, platform_application);
    GX_GET_REF_PRV(Manager, manager);
    GX_GET_PTR_PRV(FMOD::System, system);

public:
    explicit Engine(platform::Application&) noexcept;
    ~Engine() noexcept;

    void update() noexcept;
};
}
#endif