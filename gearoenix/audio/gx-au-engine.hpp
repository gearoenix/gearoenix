#ifndef GEAROENIX_AUDIO_ENGINE_HPP
#define GEAROENIX_AUDIO_ENGINE_HPP
#include "../platform/gx-plt-build-configuration.hpp"
#include "gx-au-manager.hpp"
#ifdef GX_PLATFORM_INTERFACE_SDL2
#include "sdl/gx-au-sdl-engine.hpp"
#else
#error "Not implemented audio engine for current platform interface."
#endif

namespace gearoenix::platform {
struct Application;
}

namespace gearoenix::audio {
struct Engine final {
    Manager manager;
    PlatformEngine platform_engine;

    Engine(platform::Application&) noexcept;

    void update() noexcept;
};
}
#endif