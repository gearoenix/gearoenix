#include "gx-rnd-lt-manager.hpp"
#include "gx-rnd-lt-builder.hpp"
#include "gx-rnd-lt-directional.hpp"
#include "gx-rnd-lt-point.hpp"
#include "gx-rnd-lt-light.hpp"

gearoenix::render::light::Manager::Manager(engine::Engine& e) noexcept
    : e(e)
{
    core::ecs::Component::register_type<Light>();
    core::ecs::Component::register_type<ShadowCasterDirectional>();
}

gearoenix::render::light::Manager::~Manager() noexcept = default;