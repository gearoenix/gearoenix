#include "gx-phs-anm-manager.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../render/engine/gx-rnd-eng-engine.hpp"
#include "gx-phs-anm-animation.hpp"
#include "gx-phs-anm-armature.hpp"
#include "gx-phs-anm-bone.hpp"
#include "gx-phs-anm-interpolation.hpp"

void gearoenix::physics::animation::Manager::insert_bones(
    BoneInfo& bones_info,
    const std::size_t current_index) noexcept
{
    bones[current_index].first_child_index = bones.size();
    for (auto& child_bone : bones_info.children) {
        bones_indices[child_bone.name] = bones.size();
        bones.emplace_back(
            std::move(child_bone.transform),
            std::move(child_bone.inverse_bind),
            std::move(child_bone.name),
            current_index,
            child_bone.children.size(),
            static_cast<std::size_t>(-1),
            static_cast<std::size_t>(-1));
    }
    bones[current_index].end_child_index = bones.size();
    std::size_t child_index = bones[current_index].first_child_index;
    for (auto& child_bone : bones_info.children) {
        insert_bones(child_bone, child_index);
        ++child_index;
    }
}

void gearoenix::physics::animation::Manager::animate(
    ArmatureAnimation& armature_animation,
    const double time) noexcept
{
    for (std::size_t bone_channel_index = armature_animation.bones_channels_first_index;
         bone_channel_index < armature_animation.bones_channels_end_index;
         ++bone_channel_index) {
        animate(bones_channels[bone_channel_index], time);
    }
}

void gearoenix::physics::animation::Manager::animate(
    const BoneChannel& bone_channel,
    const double time) noexcept
{
    Bone& bone = bones[bone_channel.target_bone_index];

    auto scale = bone.transform.get_scale();
    auto rotation = bone.transform.get_local_orientation();
    auto translation = bone.transform.get_local_location();

    bool transformed = false;

    if (bone_channel.scale_samples_count > 1) {
        transformed = true;
        const auto scale_begin_iter = scale_keyframes.begin() + bone_channel.scale_samples_first_keyframe_index;
        const auto scale_end_iter = scale_keyframes.begin() + bone_channel.scale_samples_end_keyframe_index;
        const auto scale_search = std::upper_bound(
            scale_begin_iter,
            scale_end_iter,
            time,
            [](const double a, const auto& b) { return a < b.first; });
        if (scale_search == scale_begin_iter)
            scale = get_key(scale_begin_iter->second);
        else if (scale_search == scale_end_iter)
            scale = get_key((scale_end_iter - 1)->second);
        else if (scale_search->first == time)
            scale = get_key(scale_search->second);
        else
            scale = interpolate(*(scale_search - 1), *scale_search, time);
    }

    if (bone_channel.rotation_samples_count > 1) {
        transformed = true;
        const auto rotation_begin_iter = rotation_keyframes.begin() + bone_channel.rotation_samples_first_keyframe_index;
        const auto rotation_end_iter = rotation_keyframes.begin() + bone_channel.rotation_samples_end_keyframe_index;
        const auto rotation_search = std::upper_bound(
            rotation_begin_iter,
            rotation_end_iter,
            time,
            [](const double a, const auto& b) { return a < b.first; });
        if (rotation_search == rotation_begin_iter)
            rotation = get_key(rotation_begin_iter->second);
        else if (rotation_search == rotation_end_iter)
            rotation = get_key((rotation_end_iter - 1)->second);
        else if (rotation_search->first == time)
            rotation = get_key(rotation_search->second);
        else
            rotation = interpolate(*(rotation_search - 1), *rotation_search, time).normalised();
    }

    if (bone_channel.translation_samples_count > 1) {
        transformed = true;
        const auto translation_begin_iter = translation_keyframes.begin() + bone_channel.translation_samples_first_keyframe_index;
        const auto translation_end_iter = translation_keyframes.begin() + bone_channel.translation_samples_end_keyframe_index;
        const auto translation_search = std::upper_bound(
            translation_begin_iter,
            translation_end_iter,
            time,
            [](const double a, const auto& b) { return a < b.first; });
        if (translation_search == translation_begin_iter)
            translation = get_key(translation_begin_iter->second);
        else if (translation_search == translation_end_iter)
            translation = get_key((translation_end_iter - 1)->second);
        else if (translation_search->first == time)
            translation = get_key(translation_search->second);
        else
            translation = interpolate(*(translation_search - 1), *translation_search, time);
    }

    if (transformed)
        bone.transform.reset(scale, rotation, translation);
}

void gearoenix::physics::animation::Manager::update_bone(const std::size_t index, const Transformation& parent_transform) noexcept
{
    auto& bone = bones[index];
    bone.transform.update_without_inverse(parent_transform);
    if (bone.transform.get_changed()) {
        bone.m = math::Mat4x4<float>(bone.transform.get_global_matrix()) * bone.inverse_bind;
        bone.inv_m = bone.m.inverted().transposed();
    }
    for (auto i = bone.first_child_index; i < bone.end_child_index; ++i) {
        update_bone(i, bone.transform);
    }
    bone.transform.clear_change();
}

gearoenix::physics::animation::Manager::Manager(render::engine::Engine& e) noexcept
    : e(e)
{
}

gearoenix::physics::animation::Manager::~Manager() noexcept = default;

void gearoenix::physics::animation::Manager::create_armature(
    core::ecs::EntityBuilder& builder,
    const std::string& name,
    BoneInfo& bones_info) noexcept
{
    GX_GUARD_LOCK(this);

    const auto root_index = bones.size();

    Armature arm;
    arm.root_bone_index = root_index;
    builder.add_component(std::move(arm));

    bones_indices[bones_info.name] = root_index;

    bones.emplace_back(
        std::move(bones_info.transform),
        std::move(bones_info.inverse_bind),
        std::move(bones_info.name),
        static_cast<std::size_t>(-1),
        bones_info.children.size(),
        static_cast<std::size_t>(-1),
        static_cast<std::size_t>(-1));

    insert_bones(bones_info, root_index);
}

void gearoenix::physics::animation::Manager::create_animation_player(
    core::ecs::EntityBuilder& builder,
    ArmatureAnimationInfo& info) noexcept
{
    GX_GUARD_LOCK(this);

    builder.add_component(AnimationPlayer(armature_animations.size()));
    if (!info.name.empty())
        armature_animations_indices[info.name] = armature_animations.size();
    ArmatureAnimation anim;
    anim.name = info.name;
    anim.bones_channels_count = info.channels.size();
    anim.bones_channels_first_index = bones_channels.size();
    for (auto& bone_channel : info.channels) {
        BoneChannel bch;
        auto bone_search = bones_indices.find(bone_channel.target_bone);
        GX_ASSERT(bones_indices.end() != bone_search);
        bch.target_bone_index = bone_search->second;

        bch.scale_samples_count = bone_channel.scale_samples.size();
        bch.scale_samples_first_keyframe_index = scale_keyframes.size();
        for (auto& k : bone_channel.scale_samples)
            scale_keyframes.push_back(k);
        bch.scale_samples_end_keyframe_index = scale_keyframes.size();

        bch.rotation_samples_count = bone_channel.rotation_samples.size();
        bch.rotation_samples_first_keyframe_index = rotation_keyframes.size();
        for (auto& k : bone_channel.rotation_samples)
            rotation_keyframes.push_back(k);
        bch.rotation_samples_end_keyframe_index = rotation_keyframes.size();

        bch.translation_samples_count = bone_channel.translation_samples.size();
        bch.translation_samples_first_keyframe_index = translation_keyframes.size();
        for (auto& k : bone_channel.translation_samples)
            translation_keyframes.push_back(k);
        bch.translation_samples_end_keyframe_index = translation_keyframes.size();

        GX_ASSERT(bch.scale_samples_count != 1);
        GX_ASSERT(bch.rotation_samples_count != 1);
        GX_ASSERT(bch.translation_samples_count != 1);

        bones_channels.push_back(bch);
    }
    anim.bones_channels_end_index = bones_channels.size();
    armature_animations.push_back(std::move(anim));
}

void gearoenix::physics::animation::Manager::update() noexcept
{
    e.get_world()->parallel_system<core::ecs::And<core::ecs::Or<AnimationPlayer, Armature>, Transformation>>([this](auto, AnimationPlayer* const player, Armature* const armature, const Transformation* const model_transform, auto) noexcept {
        if (nullptr != player) {
            player->update_time(e.get_delta_time());
            if (std::type_index(typeid(ArmatureAnimation)) == player->get_animation_type()) {
                animate(armature_animations[player->get_index()], player->time);
            } else {
                GX_UNEXPECTED;
            }
        }
        if (nullptr != armature) {
            update_bone(armature->root_bone_index, *model_transform);
        }
    });
}
