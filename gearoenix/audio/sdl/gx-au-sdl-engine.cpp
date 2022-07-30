#include "gx-au-sdl-engine.hpp"
#ifdef GX_PLATFORM_INTERFACE_SDL2
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../../platform/gx-plt-log.hpp"
#include "../gx-au-constants.hpp"

gearoenix::audio::SdlEngine::SdlEngine() noexcept
{
    GX_SET_ZERO(spec);

    SDL_AudioSpec wanted_spec;
    GX_SET_ZERO(wanted_spec);
    wanted_spec.freq = sampler_rate;
    wanted_spec.format = AUDIO_U16LSB;
    wanted_spec.channels = 1;
    wanted_spec.samples = sampler_rate / 10;

    device = SDL_OpenAudioDevice(NULL, 0, &wanted_spec, &spec, 0);
    if (device == 0) {
        GX_LOG_F("Failed to open audio: " << SDL_GetError());
    }
    SDL_PauseAudioDevice(device, 0);
}

gearoenix::audio::SdlEngine::~SdlEngine() noexcept
{
    SDL_CloseAudioDevice(device);
}

void gearoenix::audio::SdlEngine::play(const std::vector<std::uint16_t>& samples) noexcept
{
    SDL_QueueAudio(device, samples.data(), static_cast<std::uint32_t>(samples.size()));
}

int gearoenix::audio::SdlEngine::in_queue_samples_count() noexcept
{
    return static_cast<int>(SDL_GetQueuedAudioSize(device));
}

#endif