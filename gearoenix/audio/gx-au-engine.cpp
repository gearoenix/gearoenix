#include "gx-au-engine.hpp"
#include "../core/sync/gx-cr-sync-parallel-for.hpp"
#include "gx-au-macros.hpp"
#include <fmod.hpp>
#include <fmod_errors.h>
#include <random>

gearoenix::audio::Engine::Engine(platform::Application& app) noexcept
    : platform_application(app)
    , manager(*this)
{
    GX_AUDIO_FMOD_RESULT_CHECK(FMOD::System_Create(&system));
    GX_AUDIO_FMOD_RESULT_CHECK(system->init(512, FMOD_INIT_NORMAL, 0));
}

gearoenix::audio::Engine::~Engine() noexcept
{
    system->release();
}

void gearoenix::audio::Engine::update() noexcept
{
    system->update();
}
