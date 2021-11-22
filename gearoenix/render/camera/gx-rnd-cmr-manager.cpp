#include "gx-rnd-cmr-manager.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"

gearoenix::render::camera::Manager::Manager(engine::Engine& e) noexcept
    : e(e)
{
}

std::shared_ptr<gearoenix::render::camera::Builder> gearoenix::render::camera::Manager::build(const std::string& name) noexcept
{
    return std::shared_ptr<Builder>(new Builder(e, name));
}
