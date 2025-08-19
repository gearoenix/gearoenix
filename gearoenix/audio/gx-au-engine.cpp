#include "gx-au-engine.hpp"
#include "../platform/gx-plt-build-configuration.hpp"
#include "gx-au-macros.hpp"
#include <random>

gearoenix::audio::Engine::Engine()
    : Singleton(this)
{
}

gearoenix::audio::Engine::~Engine() { }

void gearoenix::audio::Engine::update() { }
