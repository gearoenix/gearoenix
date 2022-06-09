#include "gx-rnd-mdl-manager.hpp"

gearoenix::render::model::Manager::Manager(engine::Engine& e) noexcept
    : e(e)
{
}

gearoenix::render::model::Manager::~Manager() noexcept = default;