#ifndef GEAROENIX_AUDIO_ENGINE_HPP
#define GEAROENIX_AUDIO_ENGINE_HPP
#include "../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../core/sync/gx-cr-sync-semaphore.hpp"
#include "../platform/gx-plt-build-configuration.hpp"
#include "gx-au-manager.hpp"

#ifdef GX_PLATFORM_INTERFACE_SDL2
#include "sdl/gx-au-sdl-engine.hpp"
#else
#error "Not implemented audio engine for current platform interface."
#endif

#include <chrono>
#include <thread>

namespace gearoenix::platform {
struct Application;
}

namespace gearoenix::audio {
struct Engine final {
    GX_GET_RRF_PRV(platform::Application, platform_application)
    GX_GET_REF_PRV(Manager, manager)
    GX_GET_REF_PRV(PlatformEngine, platform_engine)

private:
    std::chrono::high_resolution_clock::time_point last_composition_time;
    std::vector<std::uint16_t> composited_samples;
    std::vector<std::vector<std::uint16_t>> threads_composited_samples;
    core::sync::Semaphore compositor_signal;
    bool compositor_thread_terminated = false;
    bool compositor_thread_not_joined = true;
    std::thread compositor_thread;

    void compositor_loop() noexcept;
    void composit() noexcept;

public:
    Engine(platform::Application&) noexcept;
    ~Engine() noexcept;

    void update() noexcept;
};
}
#endif