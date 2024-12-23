#pragma once
#include "../../core/allocator/gx-cr-alc-static-block.hpp"
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
    typedef std::vector<std::pair<double, Keyframe<math::Vec3<double>>>> TimeVec3Map;
    typedef std::vector<std::pair<double, Keyframe<math::Quat<double>>>> TimeQuatMap;

    render::engine::Engine& e;
    GX_GET_CREF_PRV(std::vector<BoneChannel>, bones_channels);
    GX_GET_REF_PRV(std::vector<Bone>, bones);
    GX_GET_CREF_PRV(TimeVec3Map, scale_keyframes);
    GX_GET_CREF_PRV(TimeQuatMap, rotation_keyframes);
    GX_GET_CREF_PRV(TimeVec3Map, translation_keyframes);

private:
    std::mutex this_lock;
    std::map<std::string, std::uint32_t> bones_indices;
    core::allocator::StaticBlock<1048 * 1024> animation_allocator;
    std::map<std::string, std::shared_ptr<Animation>> animations_map;

    void insert_bones(BoneInfo& bones_info, std::uint32_t current_index);
    void update_bone(std::uint32_t index, Transformation* parent);

public:
    explicit Manager(render::engine::Engine& e);
    Manager(const Manager&) = delete;
    Manager(Manager&&) = delete;
    ~Manager();
    void operator=(const Manager& o) = delete;
    void operator=(Manager&& o) = delete;
    void create_armature(core::ecs::EntityBuilder& builder, BoneInfo& bones_info);
    void create_animation_player(core::ecs::EntityBuilder& builder, ArmatureAnimationInfo& info);
    void create_sprite_player(
        core::ecs::EntityBuilder& builder,
        std::string name,
        std::shared_ptr<render::material::Sprite> sprite,
        std::uint32_t width,
        std::uint32_t height);
    void update();

    template <typename Function>
    void loop_over_bones(Function&& function, const std::uint32_t bone_index)
    {
        const Bone& bone = bones[bone_index];
        for (std::uint32_t child_index = bone.first_child_index; child_index < bone.end_child_index; ++child_index)
            function(bones[child_index]);
        for (std::uint32_t child_index = bone.first_child_index; child_index < bone.end_child_index; ++child_index)
            loop_over_bones(function, child_index);
    }

    template <typename Function>
    void loop_over_bones(Function&& function, Armature& armature)
    {
        function(bones[armature.root_bone_index]);
        loop_over_bones(function, armature.root_bone_index);
    }
};
}