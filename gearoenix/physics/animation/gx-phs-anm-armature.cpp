#include "gx-phs-anm-armature.hpp"

gearoenix::physics::animation::Armature::Armature(std::string&& name) noexcept
    : core::ecs::Component(this, std::move(name))
{
}

gearoenix::physics::animation::Armature::Armature(Armature&& o) noexcept = default;

gearoenix::physics::animation::Armature::~Armature() noexcept = default;
