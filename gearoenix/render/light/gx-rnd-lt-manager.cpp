#include "gx-rnd-lt-manager.hpp"
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "gx-rnd-lt-directional.hpp"
#include "gx-rnd-lt-point.hpp"

gearoenix::render::light::Manager::Manager(engine::Engine& e)
    : e(e)
{
    core::ecs::ComponentType::add<Light>();
    core::ecs::ComponentType::add<Directional>();
    core::ecs::ComponentType::add<Point>();
    core::ecs::ComponentType::add<ShadowCasterDirectional>();
}

gearoenix::render::light::Manager::~Manager() = default;