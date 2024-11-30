#include "gx-phs-anm-animation.hpp"
#include "../../render/material/gx-rnd-mat-sprite.hpp"
#include "gx-phs-anm-interpolation.hpp"
#include "gx-phs-anm-manager.hpp"
#include <algorithm>
#include <utility>

void gearoenix::physics::animation::ArmatureAnimationInfo::optimise()
{
    for (auto& c : channels)
        c.optimise();
}

gearoenix::physics::animation::Animation::Animation(std::string name)
    : name(std::move(name))
{
}

gearoenix::physics::animation::Animation::~Animation() = default;

gearoenix::physics::animation::ArmatureAnimation::ArmatureAnimation(
    std::string name,
    const std::uint32_t bones_channels_count,
    const std::uint32_t bones_channels_first_index,
    const std::uint32_t bones_channels_end_index)
    : Animation(std::move(name))
    , bones_channels_count(bones_channels_count)
    , bones_channels_first_index(bones_channels_first_index)
    , bones_channels_end_index(bones_channels_end_index)
{
}

gearoenix::physics::animation::ArmatureAnimation::~ArmatureAnimation() = default;

void gearoenix::physics::animation::ArmatureAnimation::animate(Manager& manager, const double time)
{
    for (std::uint32_t bone_channel_index = bones_channels_first_index; bone_channel_index < bones_channels_end_index; ++bone_channel_index) {
        animate(manager, manager.get_bones_channels()[bone_channel_index], time);
    }
}

void gearoenix::physics::animation::ArmatureAnimation::animate(Manager& manager, const BoneChannel& bone_channel, const double time)
{
    Bone& bone = manager.get_bones()[bone_channel.target_bone_index];

    auto scale = bone.transform->get_scale();
    auto rotation = bone.transform->get_rotation();
    auto translation = bone.transform->get_local_position();

    bool transformed = false;

    if (bone_channel.scale_samples_count > 0) {
        transformed = true;
        const auto scale_begin_iter = manager.get_scale_keyframes().begin() + static_cast<Manager::TimeVec3Map::difference_type>(bone_channel.scale_samples_first_keyframe_index);
        const auto scale_end_iter = manager.get_scale_keyframes().begin() + static_cast<Manager::TimeVec3Map::difference_type>(bone_channel.scale_samples_end_keyframe_index);
        const auto scale_search = std::upper_bound(
            scale_begin_iter,
            scale_end_iter,
            time,
            [](const double a, const auto& b) { return a < b.first; });
        if (scale_search == scale_begin_iter) {
            scale = get_key(scale_begin_iter->second);
        } else if (scale_search == scale_end_iter) {
            scale = get_key((scale_begin_iter + (static_cast<Manager::TimeVec3Map::difference_type>(bone_channel.scale_samples_count) - 1))->second);
        } else if (scale_search->first == time) {
            scale = get_key(scale_search->second);
        } else {
            scale = interpolate(*(scale_search - 1), *scale_search, time);
        }
    }

    if (bone_channel.rotation_samples_count > 0) {
        transformed = true;
        const auto rotation_begin_iter = manager.get_rotation_keyframes().begin() + static_cast<Manager::TimeQuatMap::difference_type>(bone_channel.rotation_samples_first_keyframe_index);
        const auto rotation_end_iter = manager.get_rotation_keyframes().begin() + static_cast<Manager::TimeQuatMap::difference_type>(bone_channel.rotation_samples_end_keyframe_index);
        const auto rotation_search = std::upper_bound(
            rotation_begin_iter,
            rotation_end_iter,
            time,
            [](const double a, const auto& b) { return a < b.first; });
        if (rotation_search == rotation_begin_iter) {
            rotation = get_key(rotation_begin_iter->second);
        } else if (rotation_search == rotation_end_iter) {
            rotation = get_key((rotation_begin_iter + (static_cast<Manager::TimeQuatMap::difference_type>(bone_channel.rotation_samples_count) - 1))->second);
        } else if (rotation_search->first == time) {
            rotation = get_key(rotation_search->second);
        } else {
            rotation = interpolate(*(rotation_search - 1), *rotation_search, time).normalised();
        }
    }

    if (bone_channel.translation_samples_count > 0) {
        transformed = true;
        const auto translation_begin_iter = manager.get_translation_keyframes().begin() + static_cast<Manager::TimeVec3Map::difference_type>(bone_channel.translation_samples_first_keyframe_index);
        const auto translation_end_iter = manager.get_translation_keyframes().begin() + static_cast<Manager::TimeVec3Map::difference_type>(bone_channel.translation_samples_end_keyframe_index);
        const auto translation_search = std::upper_bound(
            translation_begin_iter,
            translation_end_iter,
            time,
            [](const double a, const auto& b) { return a < b.first; });
        if (translation_search == translation_begin_iter) {
            translation = get_key(translation_begin_iter->second);
        } else if (translation_search == translation_end_iter) {
            translation = get_key((translation_begin_iter + (static_cast<Manager::TimeVec3Map::difference_type>(bone_channel.translation_samples_count) - 1))->second);
        } else if (translation_search->first == time) {
            translation = get_key(translation_search->second);
        } else {
            translation = interpolate(*(translation_search - 1), *translation_search, time);
        }
    }
    if (transformed) {
        bone.transform->reset(scale, rotation, translation);
    }
}

gearoenix::physics::animation::SpriteAnimation::SpriteAnimation(
    std::string name,
    std::shared_ptr<render::material::Sprite> sprite,
    const std::uint32_t width,
    const std::uint32_t height)
    : Animation(std::move(name))
    , sprite(std::move(sprite))
    , count(static_cast<double>(width * height))
    , width(width)
    , height(height)
    , uv_scale(1.0 / static_cast<double>(width), 1.0f / static_cast<double>(height))
{
}

void gearoenix::physics::animation::SpriteAnimation::animate(
    Manager&, const double time)
{
    GX_ASSERT_D(time <= 1.0);
    const auto pos = static_cast<std::uint32_t>(count * time);
    const math::Vec2 v(static_cast<double>(pos % width), static_cast<double>(pos / width));
    sprite->get_uv_transform() = math::Vec4<float>(uv_scale, uv_scale * v);
}

gearoenix::physics::animation::SpriteAnimation::~SpriteAnimation() = default;

gearoenix::physics::animation::AnimationPlayer::AnimationPlayer(
    std::shared_ptr<Animation> animation,
    std::string&& name,
    const double starting_time,
    const core::ecs::entity_id_t entity_id)
    : Component(create_this_type_index(this), std::move(name), entity_id)
    , time(starting_time)
    , animation(std::move(animation))
{
}

gearoenix::physics::animation::AnimationPlayer::~AnimationPlayer() = default;

void gearoenix::physics::animation::AnimationPlayer::update_time(const double delta_time)
{
    time += delta_time * speed;
    if (is_loop) {
        time -= loop_start_time;
        time = std::fmod(time, loop_length_time);
        time += loop_start_time;
    }
}

void gearoenix::physics::animation::AnimationPlayer::set_loop_start_time(const double t)
{
    loop_start_time = t;
    loop_length_time = loop_end_time - loop_start_time;
}

void gearoenix::physics::animation::AnimationPlayer::set_loop_end_time(const double t)
{
    loop_end_time = t;
    loop_length_time = loop_end_time - loop_start_time;
}

void gearoenix::physics::animation::AnimationPlayer::set_loop_range_time(const double start, const double end)
{
    loop_start_time = start;
    loop_end_time = end;
    loop_length_time = loop_end_time - loop_start_time;
}

void gearoenix::physics::animation::AnimationPlayer::animate(Manager& manager)
{
    animation->animate(manager, time);
}