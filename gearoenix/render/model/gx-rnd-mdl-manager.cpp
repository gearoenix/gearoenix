#include "gx-rnd-mdl-manager.hpp"
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "gx-rnd-mdl-model.hpp"

gearoenix::render::model::Manager::Manager(engine::Engine& e)
    : e(e)
{
    core::ecs::ComponentType::add<Model>();
}

gearoenix::render::model::Manager::~Manager() = default;