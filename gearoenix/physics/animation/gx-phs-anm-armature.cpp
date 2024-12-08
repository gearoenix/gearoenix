#include "gx-phs-anm-armature.hpp"
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"

gearoenix::physics::animation::Armature::Armature(std::string&& name, const core::ecs::entity_id_t entity_id)
    : Component(core::ecs::ComponentType::create_index(this), std::move(name), entity_id)
{
}

gearoenix::physics::animation::Armature::~Armature() = default;