#include "gx-phs-anm-armature.hpp"

gearoenix::physics::animation::Armature::Armature() noexcept
	: core::ecs::Component(this)
{
}

gearoenix::physics::animation::Armature::Armature(Armature&&) noexcept = default;

gearoenix::physics::animation::Armature::~Armature() noexcept = default;
