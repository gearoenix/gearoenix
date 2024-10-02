#include "gx-phs-anm-manager.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../render/engine/gx-rnd-eng-engine.hpp"
#include "gx-phs-anm-animation.hpp"
#include "gx-phs-anm-armature.hpp"
#include "gx-phs-anm-bone.hpp"

void gearoenix::physics::animation::Manager::insert_bones(
    BoneInfo& bones_info,
    const std::size_t current_index)
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

void gearoenix::physics::animation::Manager::update_bone(const std::size_t index, const Transformation& parent_transform)
{
    auto& bone = bones[index];
    bone.transform.set_parent(&parent_transform);
    bone.transform.update_without_inverse_child();
    if (bone.transform.get_changed()) {
        bone.m = math::Mat4x4<float>(bone.transform.get_global_matrix()) * bone.inverse_bind;
        bone.inv_m = bone.m.inverted().transposed();
    }
    for (auto i = bone.first_child_index; i < bone.end_child_index; ++i) {
        update_bone(i, bone.transform);
    }
    bone.transform.clear_change();
}

gearoenix::physics::animation::Manager::Manager(render::engine::Engine& e)
    : e(e)
{
}

gearoenix::physics::animation::Manager::~Manager() = default;

void gearoenix::physics::animation::Manager::create_armature(
    core::ecs::EntityBuilder& builder,
    BoneInfo& bones_info)
{
    const std::lock_guard _lg(this_lock);

    const auto root_index = bones.size();

    auto arm = Armature::construct(builder.get_name() + "-armature", builder.get_id());
    arm->root_bone_index = root_index;
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
    ArmatureAnimationInfo& info)
{
    const std::lock_guard _lg(this_lock);
    const std::size_t bones_channels_count = info.channels.size();
    const std::size_t bones_channels_first_index = bones_channels.size();
    for (auto& bone_channel : info.channels) {
        BoneChannel bch;
        auto bone_search = bones_indices.find(bone_channel.target_bone);
        GX_ASSERT(bones_indices.end() != bone_search);
        bch.target_bone_index = bone_search->second;
        bch.scale_samples_count = bone_channel.scale_samples.size();
        bch.scale_samples_first_keyframe_index = scale_keyframes.size();
        double last_time = -0.1e-10;
        for (auto& k : bone_channel.scale_samples) {
            GX_ASSERT(last_time < k.first);
            last_time = k.first;
            scale_keyframes.push_back(k);
        }
        bch.scale_samples_end_keyframe_index = scale_keyframes.size();
        bch.rotation_samples_count = bone_channel.rotation_samples.size();
        bch.rotation_samples_first_keyframe_index = rotation_keyframes.size();
        last_time = -0.1e-10;
        for (auto& k : bone_channel.rotation_samples) {
            GX_ASSERT(last_time < k.first);
            last_time = k.first;
            rotation_keyframes.push_back(k);
        }
        bch.rotation_samples_end_keyframe_index = rotation_keyframes.size();
        bch.translation_samples_count = bone_channel.translation_samples.size();
        bch.translation_samples_first_keyframe_index = translation_keyframes.size();
        last_time = -0.1e-10;
        for (auto& k : bone_channel.translation_samples) {
            GX_ASSERT(last_time < k.first);
            last_time = k.first;
            translation_keyframes.push_back(k);
        }
        bch.translation_samples_end_keyframe_index = translation_keyframes.size();
        bones_channels.push_back(bch);
    }
    const std::size_t bones_channels_end_index = bones_channels.size();
    auto anim = animation_allocator.make_shared<ArmatureAnimation>(info.name, bones_channels_count, bones_channels_first_index, bones_channels_end_index);
    if (!info.name.empty()) {
        animations_map.emplace(info.name, anim);
    }
    builder.add_component(AnimationPlayer::construct(std::move(anim), builder.get_name() + "-animation-player", 0.0, builder.get_id()));
}

void gearoenix::physics::animation::Manager::create_sprite_player(
    core::ecs::EntityBuilder& builder,
    std::string name,
    std::shared_ptr<render::material::Sprite> sprite,
    const std::size_t width,
    const std::size_t height)
{
    auto anim = animation_allocator.make_shared<SpriteAnimation>(name, std::move(sprite), width, height);
    if (!name.empty()) {
        animations_map.emplace(std::move(name), anim);
    }
    auto player = AnimationPlayer::construct(std::move(anim), builder.get_name() + "-animation-player", 0.0, builder.get_id());
    player->set_loop_range_time(0.0, 0.999);
    builder.add_component(std::move(player));
}

void gearoenix::physics::animation::Manager::update()
{
    e.get_world()->parallel_system<core::ecs::All<core::ecs::Any<AnimationPlayer, Armature>, TransformationComponent>>(
        [this](auto, AnimationPlayer* const player, Armature* const armature, const TransformationComponent* const model_transform, auto) {
            if (nullptr != player) {
                player->update_time(e.get_delta_time());
                player->animate(*this);
            }
            if (nullptr != armature) {
                update_bone(armature->root_bone_index, *model_transform);
            }
        });
}
