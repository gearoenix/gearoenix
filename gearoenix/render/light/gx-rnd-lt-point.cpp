#include "gx-rnd-lt-point.hpp"
#include "../../core/allocator/gx-cr-alc-shared-array.hpp"

namespace {
const auto allocator = gearoenix::core::allocator::SharedArray<gearoenix::render::light::Point, 16>::construct();
}

const gearoenix::core::ecs::Component::HierarchyTypes& gearoenix::render::light::Point::get_hierarchy_types() const
{
    static const auto types = generate_hierarchy_types<Light>(this);
    return types;
}

gearoenix::render::light::Point::Point(std::string&& name, const core::ecs::entity_id_t entity_id)
    : Light(create_this_type_index(this), std::move(name), entity_id)
{
}

std::shared_ptr<gearoenix::render::light::Point> gearoenix::render::light::Point::construct(
    std::string&& name, const core::ecs::entity_id_t entity_id)
{
    auto self = allocator->make_shared(std::move(name), entity_id);
    self->set_component_self(self);
    return self;
}

gearoenix::render::light::Point::~Point() = default;