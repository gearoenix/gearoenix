#include "gx-phs-anm-armature.hpp"
#include "../../core/allocator/gx-cr-alc-shared-array.hpp"

namespace {
const auto allocator = gearoenix::core::allocator::SharedArray<gearoenix::physics::animation::Armature, 16>::construct();
}

gearoenix::physics::animation::Armature::Armature(std::string&& name, const core::ecs::entity_id_t entity_id)
    : Component(create_this_type_index(this), std::move(name), entity_id)
{
}

std::shared_ptr<gearoenix::physics::animation::Armature> gearoenix::physics::animation::Armature::construct(
    std::string&& name, const core::ecs::entity_id_t entity_id)
{
    auto self = allocator->make_shared(std::move(name), entity_id);
    self->set_component_self(self);
    return self;
}

gearoenix::physics::animation::Armature::~Armature() = default;

const gearoenix::core::ecs::Component::HierarchyTypes& gearoenix::physics::animation::Armature::get_hierarchy_types() const
{
    static const auto types = generate_hierarchy_types(this);
    return types;
}