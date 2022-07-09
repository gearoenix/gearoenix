#ifndef GEAROENIX_PHYSICS_ANIMATION_MANAGER_HPP
#define GEAROENIX_PHYSICS_ANIMATION_MANAGER_HPP
#include "../../platform/macro/gx-plt-mcr-lock.hpp"
#include "gx-phs-anm-animation.hpp"
#include "gx-phs-anm-armature.hpp"
#include "gx-phs-anm-bone.hpp"
#include "gx-phs-anm-channel.hpp"
#include <map>
#include <string>
#include <vector>

namespace gearoenix::core::ecs {
struct EntityBuilder;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::physics::animation {
struct Armature;
struct Manager final {
    render::engine::Engine& e;

private:
    GX_CREATE_GUARD(this);

    std::vector<Bone> bones;
    std::map<std::string, std::size_t> bones_indices;

    std::vector<ArmatureAnimation> armature_animations;
    std::map<std::string, std::size_t> armature_animations_indices;
    std::vector<BoneChannel> bones_channels;
    std::vector<std::pair<double, Keyframe<math::Vec3<double>>>> scale_keyframes;
    std::vector<std::pair<double, Keyframe<math::Quat<double>>>> rotation_keyframes;
    std::vector<std::pair<double, Keyframe<math::Vec3<double>>>> translation_keyframes;

    void insert_bones(BoneInfo& bones_info, std::size_t current_index) noexcept;
    void animate(ArmatureAnimation& armature_animation, double time) noexcept;
    void animate(const BoneChannel& bone_channel, double time) noexcept;
    void update_bone(std::size_t index, const Transformation& parent) noexcept;

public:
    explicit Manager(render::engine::Engine& e) noexcept;
    Manager(const Manager&) = delete;
    Manager(Manager&&) = delete;
    ~Manager() noexcept;
    void operator=(const Manager& o) = delete;
    void operator=(Manager&& o) = delete;
    void create_armature(core::ecs::EntityBuilder& builder, const std::string& name, BoneInfo& bones_info) noexcept;
    void create_animation_player(core::ecs::EntityBuilder& builder, ArmatureAnimationInfo& info) noexcept;
    void update() noexcept;

    template <typename Function>
    void loop_over_bones(Function&& function, const std::size_t bone_index) noexcept
    {
        Bone& bone = bones[bone_index];
        function(bone);
        for (std::size_t child_index = bone.first_child_index; child_index < bone.end_child_index; ++child_index)
            loop_over_bones(function, child_index);
    }

    template <typename Function>
    void loop_over_bones(Function&& function, Armature& armature) noexcept
    {
        loop_over_bones(function, armature.root_bone_index);
    }
};
}
#endif