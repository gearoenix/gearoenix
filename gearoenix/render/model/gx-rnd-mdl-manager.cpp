#include "gx-rnd-mdl-manager.hpp"
#include "gx-rnd-mdl-model.hpp"

gearoenix::render::model::Manager::Manager(engine::Engine& e)
    : e(e)
{
    core::ecs::Component::register_type<Model>();
}

gearoenix::render::model::Manager::~Manager() = default;