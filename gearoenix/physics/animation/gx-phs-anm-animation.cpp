#include "gx-phs-anm-animation.hpp"
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../../core/gx-cr-string.hpp"
#include "../../render/imgui/gx-rnd-imgui-type-table.hpp"
#include "../../render/imgui/gx-rnd-imgui-type-tree.hpp"
#include "../../render/material/gx-rnd-mat-sprite.hpp"
#include "../gx-phs-transformation.hpp"
#include "gx-phs-anm-bone.hpp"
#include "gx-phs-anm-interpolation.hpp"
#include "gx-phs-anm-manager.hpp"
#include <algorithm>
#include <utility>

gearoenix::physics::animation::Animation::Animation(std::string&& name)
    : name(std::move(name))
{
}

gearoenix::physics::animation::Animation::~Animation() = default;

void gearoenix::physics::animation::Animation::show_debug_gui()
{
    render::imgui::tree_scope(this, [this] {
        render::imgui::table_scope(
            "##gearoenix::physics::animation::Animation::show_debug_gui",
            [this] {
                ImGui::Text("Name:");
                ImGui::TableNextColumn();
                ImGui::Text("%s", name.c_str());
            });
    });
}

gearoenix::physics::animation::ArmatureAnimation::ArmatureAnimation(std::string&& name, std::shared_ptr<Bone>&& root_bone)
    : Animation(std::move(name))
    , root_bone(std::move(root_bone))
{
}

gearoenix::physics::animation::ArmatureAnimation::~ArmatureAnimation() = default;

void gearoenix::physics::animation::ArmatureAnimation::animate(const double time)
{
    animate(*root_bone, time);
}

void gearoenix::physics::animation::ArmatureAnimation::animate(const Bone& bone, const double time)
{
    auto& bone_transform = *bone.get_transform();
    auto scale = bone_transform.get_scale();
    auto rotation = bone_transform.get_rotation();
    auto translation = bone_transform.get_local_position();

    bool transformed = false;

    if (const auto& scale_samples = bone.get_scale_samples(); !scale_samples.empty()) {
        transformed = true;
        if (const auto scale_search = bone.get_scale_samples().upper_bound(time); scale_search == scale_samples.begin() || scale_search->first == time) {
            scale = scale_search->second.get_key();
        } else if (scale_search == scale_samples.end()) {
            scale = scale_samples.rbegin()->second.get_key();
        } else {
            scale = interpolate(*(scale_search - 1), *scale_search, time);
        }
    }

    if (const auto& rotation_samples = bone.get_rotation_samples(); !rotation_samples.empty()) {
        transformed = true;
        if (const auto rotation_search = rotation_samples.upper_bound(time); rotation_search == rotation_samples.begin() || rotation_search->first == time) {
            rotation = rotation_search->second.get_key();
        } else if (rotation_search == rotation_samples.end()) {
            rotation = rotation_samples.rbegin()->second.get_key();
        } else {
            rotation = interpolate(*(rotation_search - 1), *rotation_search, time).normalised();
        }
    }

    if (const auto& translation_samples = bone.get_translation_samples(); !translation_samples.empty()) {
        transformed = true;
        if (const auto translation_search = translation_samples.upper_bound(time); translation_search == translation_samples.begin() || translation_search->first == time) {
            translation = translation_search->second.get_key();
        } else if (translation_search == translation_samples.end()) {
            translation = translation_samples.rbegin()->second.get_key();
        } else {
            translation = interpolate(*(translation_search - 1), *translation_search, time);
        }
    }
    if (transformed) {
        bone_transform.reset(scale, rotation, translation);
    }

    for (const auto* const child : bone.get_children()) {
        animate(*child, time);
    }
}

void gearoenix::physics::animation::ArmatureAnimation::show_debug_gui()
{
    render::imgui::tree_scope(this, [this] {
        Animation::show_debug_gui();
        render::imgui::table_scope(
            "##gearoenix::physics::animation::ArmatureAnimation::show_debug_gui",
            [this] {
                ImGui::Text("Root Bone Entity ID:");
                ImGui::TableNextColumn();
                ImGui::Text("%ul", root_bone->get_entity_id());
            });
        root_bone->show_debug_gui();
    });
}

gearoenix::physics::animation::SpriteAnimation::SpriteAnimation(
    std::string&& name,
    std::shared_ptr<render::material::Sprite>&& sprite,
    const std::uint32_t width,
    const std::uint32_t height)
    : Animation(std::move(name))
    , sprite(std::move(sprite))
    , count(static_cast<double>(width * height))
    , aspect(width, height)
    , uv_scale(1.0 / static_cast<double>(width), 1.0f / static_cast<double>(height))
{
}

void gearoenix::physics::animation::SpriteAnimation::animate(const double time)
{
    GX_ASSERT_D(time <= 1.0);
    const auto pos = static_cast<std::uint32_t>(count * time);
    const math::Vec2 v(static_cast<double>(pos % aspect.x), static_cast<double>(pos / aspect.y));
    sprite->get_uv_transform() = math::Vec4<float>(uv_scale, uv_scale * v);
}

void gearoenix::physics::animation::SpriteAnimation::show_debug_gui()
{
    render::imgui::tree_scope(this, [this] {
        Animation::show_debug_gui();
        render::imgui::table_scope(
            "##gearoenix::physics::animation::SpriteAnimation::show_debug_gui",
            [this] {
                ImGui::Text("Count:");
                ImGui::TableNextColumn();
                ImGui::Text("%ul", count);
            });
    });
}

gearoenix::physics::animation::SpriteAnimation::~SpriteAnimation() = default;

void gearoenix::physics::animation::AnimationPlayer::show_debug_gui()
{
    render::imgui::tree_scope(this, [this] {
        Component::show_debug_gui();
        render::imgui::table_scope(
            "##gearoenix::physics::animation::AnimationPlayer::show_debug_gui", [this] {
                ImGui::Text("Time: ");
                ImGui::TableNextColumn();
                ImGui::InputDouble("##time", &time);
                ImGui::TableNextColumn();

                ImGui::Text("Speed: ");
                ImGui::TableNextColumn();
                ImGui::InputDouble("##speed", &speed);
                ImGui::TableNextColumn();

                ImGui::Text("Is Loop: ");
                ImGui::TableNextColumn();
                ImGui::Checkbox("##is_loop", &is_loop);
                ImGui::TableNextColumn();

                ImGui::Text("Loop Start Time: ");
                ImGui::TableNextColumn();
                ImGui::InputDouble("##loop_start_time", &loop_start_time);
                ImGui::TableNextColumn();

                ImGui::Text("Loop End Time: ");
                ImGui::TableNextColumn();
                ImGui::InputDouble("##loop_end_time", &loop_end_time);
                ImGui::TableNextColumn();

                loop_end_time = std::max(loop_start_time, loop_end_time);

                ImGui::Text("Loop Time: ");
                ImGui::TableNextColumn();
                ImGui::Text("%d", loop_length_time);
            });
    });
}

gearoenix::physics::animation::AnimationPlayer::AnimationPlayer(
    std::shared_ptr<Animation>&& animation,
    std::string&& name,
    const double starting_time,
    const core::ecs::entity_id_t entity_id)
    : Component(core::ecs::ComponentType::create_index(this), std::move(name), entity_id)
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

void gearoenix::physics::animation::AnimationPlayer::animate() const
{
    animation->animate(time);
}