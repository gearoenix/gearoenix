#ifndef GEAROENIX_AUDIO_SDL_ENGINE_HPP
#define GEAROENIX_AUDIO_SDL_ENGINE_HPP
#include "../../platform/gx-plt-build-configuration.hpp"
#ifdef GX_PLATFORM_INTERFACE_SDL2
#ifdef GX_PLATFORM_LINUX
#include <SDL2/SDL_audio.h>
#else
#include <SDL_audio.h>
#endif
#include <vector>

namespace gearoenix::audio {
struct SdlEngine final {
    SDL_AudioSpec spec;
    SDL_AudioDeviceID device;

    SdlEngine() noexcept;
    ~SdlEngine() noexcept;

    SdlEngine(SdlEngine&&) = delete;
    SdlEngine(const SdlEngine&) = delete;

    void play(const std::vector<std::uint16_t>& samples) noexcept;
};

using PlatformEngine = SdlEngine;
}

#endif
#endif