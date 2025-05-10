#include "gx-au-engine.hpp"
#include "../platform/gx-plt-build-configuration.hpp"
#include "gx-au-macros.hpp"
#include <random>

#if !GX_PLATFORM_WEBASSEMBLY
#include <fmod/fmod.hpp>
#include <fmod/fmod_errors.h>
#endif

gearoenix::audio::Engine::Engine()
    : Singleton(this)
    , manager(*this)
{
#if !GX_PLATFORM_WEBASSEMBLY
    GX_AUDIO_FMOD_RESULT_CHECK(FMOD::System_Create(&system));
    GX_AUDIO_FMOD_RESULT_CHECK(system->init(512, FMOD_INIT_NORMAL, 0));
#endif
}

gearoenix::audio::Engine::~Engine()
{
#if !GX_PLATFORM_WEBASSEMBLY
    system->release();
#endif
}

void gearoenix::audio::Engine::update()
{
#if !GX_PLATFORM_WEBASSEMBLY
    system->update();
#endif
}
