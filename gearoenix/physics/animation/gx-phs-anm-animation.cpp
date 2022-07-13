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

void gearoenix::physics::animation::AnimationPlayer::update_time(const double delta_time) noexcept
{
    time += delta_time;
    if (is_loop) {
        time -= loop_start_time;
        time = std::fmod(time, loop_length_time);
        time += loop_start_time;
    }
}