#include "gx-rnd-lt-point.hpp"
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"

gearoenix::render::light::Point::Point(std::string&& name, const core::ecs::entity_id_t entity_id)
    : Light(core::ecs::ComponentType::create_index(this), std::move(name), entity_id)
{
}

gearoenix::render::light::Point::~Point() = default;