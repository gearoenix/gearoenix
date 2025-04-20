#include "gx-rnd-lt-point.hpp"
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"

gearoenix::render::light::Point::Point(std::string&& name)
    : Light(core::ecs::ComponentType::create_index(this), std::move(name))
{
}

gearoenix::render::light::Point::~Point() = default;