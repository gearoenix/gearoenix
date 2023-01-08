#include "gx-rnd-lt-point.hpp"

gearoenix::render::light::Point::Point(std::string&& name) noexcept
    : core::ecs::Component(this, std::move(name))
{
}

gearoenix::render::light::Point::Point(Point&&) noexcept = default;

gearoenix::render::light::Point::~Point() noexcept = default;