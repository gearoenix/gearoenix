#include "gx-phs-anm-animation.hpp"

gearoenix::physics::animation::AnimationPlayer::AnimationPlayer(
    std::size_t index,
    const std::type_index animation_type,
    const double starting_time) noexcept
    : core::ecs::Component(this)
    , index(index)
    , time(starting_time)
    , animation_type(animation_type)
{
}

gearoenix::physics::animation::AnimationPlayer::AnimationPlayer(AnimationPlayer&&) noexcept = default;

gearoenix::physics::animation::AnimationPlayer::~AnimationPlayer() noexcept = default;
