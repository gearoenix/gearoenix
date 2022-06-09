#include "gx-rnd-lt-manager.hpp"
#include "gx-rnd-lt-builder.hpp"
#include "gx-rnd-lt-directional.hpp"
#include "gx-rnd-lt-point.hpp"

gearoenix::render::light::Manager::Manager(engine::Engine& e) noexcept
    : e(e)
{
}

gearoenix::render::light::Manager::~Manager() noexcept = default;