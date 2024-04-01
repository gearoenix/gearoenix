#include "gx-au-engine.hpp"
#include "gx-au-macros.hpp"
#include <fmod/fmod.hpp>
#include <fmod/fmod_errors.h>
#include <random>

gearoenix::audio::Engine::Engine(platform::Application& app)
    : platform_application(app)
    , manager(*this)
{
    GX_AUDIO_FMOD_RESULT_CHECK(FMOD::System_Create(&system));
    GX_AUDIO_FMOD_RESULT_CHECK(system->init(512, FMOD_INIT_NORMAL, 0));
}

gearoenix::audio::Engine::~Engine()
{
    system->release();
}

void gearoenix::audio::Engine::update()
{
    system->update();
}
