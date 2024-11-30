#include "gx-phs-anm-armature.hpp"

gearoenix::physics::animation::Armature::Armature(std::string&& name, const core::ecs::entity_id_t entity_id)
    : Component(create_this_type_index(this), std::move(name), entity_id)
{
}

gearoenix::physics::animation::Armature::~Armature() = default;