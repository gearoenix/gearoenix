#include "gx-rnd-buf-manager.hpp"

gearoenix::render::buffer::Manager::Manager()
    : Singleton(this)
{
}

gearoenix::render::buffer::Manager::~Manager() = default;
