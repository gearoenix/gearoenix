#include "gx-rnd-lt-manager.hpp"
#include "gx-rnd-lt-directional.hpp"
#include "gx-rnd-lt-point.hpp"

gearoenix::render::light::Manager::Manager(engine::Engine& e)
    : e(e)
{
    core::ecs::Component::register_type<Light>();
    core::ecs::Component::register_type<Directional>();
    core::ecs::Component::register_type<Point>();
    core::ecs::Component::register_type<ShadowCasterDirectional>();
}

gearoenix::render::light::Manager::~Manager() = default;