#include "gx-rnd-lt-point.hpp"

gearoenix::render::light::Point::Point(std::string&& name, const core::ecs::entity_id_t entity_id)
    : Light(create_this_type_index(this), std::move(name), entity_id)
{
}

gearoenix::render::light::Point::~Point() = default;