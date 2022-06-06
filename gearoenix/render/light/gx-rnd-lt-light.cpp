#include "gx-rnd-lt-light.hpp"

gearoenix::render::light::Light::Light() noexcept
    : core::ecs::Component(this)
{
}

gearoenix::render::light::Light::Light(Light&&) noexcept = default;

gearoenix::render::light::Light::~Light() noexcept = default;