#include "gx-rnd-lt-point.hpp"

gearoenix::render::light::Point::Point() noexcept
    : core::ecs::Component(this)
{
}

gearoenix::render::light::Point::Point(Point&&) noexcept = default;

gearoenix::render::light::Point::~Point() noexcept = default;