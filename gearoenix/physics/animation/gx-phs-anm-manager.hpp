#ifndef GEAROENIX_PHYSICS_ANIMATION_MANAGER_HPP
#define GEAROENIX_PHYSICS_ANIMATION_MANAGER_HPP
#include "../../platform/macro/gx-plt-mcr-lock.hpp"
#include "gx-phs-anm-armature.hpp"
#include "gx-phs-anm-bone.hpp"
#include <string>
#include <vector>

namespace gearoenix::core::ecs {
struct EntityBuilder;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::physics::animation {
struct Animation;
struct Armature;
struct Manager final {
    render::engine::Engine& e;

private:
    GX_CREATE_GUARD(bones);
    std::vector<std::uint8_t> bones;
    std::vector<Bone::Index> bone_indices;

public:
    explicit Manager(render::engine::Engine& e) noexcept;
    Manager(const Manager&) = delete;
    Manager(Manager&&) = delete;
    ~Manager() noexcept;
    void operator=(const Manager& o) = delete;
    void operator=(Manager&& o) = delete;
    void create_armature(core::ecs::EntityBuilder& builder, const std::string& name, const std::vector<std::uint8_t>& armature_bones) noexcept;
    void update() noexcept;

    template <typename Function>
    void loop_over_bones(Function&& function, std::size_t bone_index) noexcept
    {
        auto& bone = *reinterpret_cast<Bone*>(&bones[bone_index]);
        function(bone);
        bone_index += sizeof(Bone);
        for (Bone::Index child_index = 0; child_index < bone.children_count; ++child_index, bone_index += sizeof(Bone::Index))
            loop_over_bones(function, *reinterpret_cast<Bone::Index*>(&bones[bone_index]));
    }

    template <typename Function>
    void loop_over_bones(Function&& function, Armature& armature) noexcept
    {
        loop_over_bones(function, armature.root_bone_index);
    }
};
}
#endif