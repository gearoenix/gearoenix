#include "gx-phs-anm-armature.hpp"

gearoenix::physics::animation::Armature::Armature() noexcept
    : core::ecs::Component(this)
{
}

gearoenix::physics::animation::Armature::Armature(Armature&& o) noexcept = default;

gearoenix::physics::animation::Armature::~Armature() noexcept = default;
