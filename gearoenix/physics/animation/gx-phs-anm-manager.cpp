#include "gx-phs-anm-manager.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "gx-phs-anm-bone.hpp"
#include "gx-phs-anm-armature.hpp"
#include "../../render/engine/gx-rnd-eng-engine.hpp"
#include "../../core/sync/gx-cr-sync-parallel-for.hpp"

gearoenix::physics::animation::Manager::Manager(render::engine::Engine& e) noexcept
    : e(e)
{}

gearoenix::physics::animation::Manager::~Manager() noexcept = default;

void gearoenix::physics::animation::Manager::create_armature(
    core::ecs::EntityBuilder& builder,
    const std::string& name,
    const std::vector<std::uint8_t>& armature_bones) noexcept
{
    GX_GUARD_LOCK(bones);
    const auto root_bone_ptr = static_cast<Bone::Index>(bones.size());
    const auto src_last_ptr = static_cast<Bone::Index>(armature_bones.size());
    bones.resize(bones.size() + armature_bones.size());
    for (Bone::Index src_ptr = 0, dst_ptr = root_bone_ptr; src_ptr < src_last_ptr;)
    {
        bone_indices.push_back(dst_ptr);
        auto* const bone_dst_ptr = &bones[dst_ptr];
        std::memcpy(bone_dst_ptr, &armature_bones[src_ptr], sizeof(Bone));
        auto& bone = *reinterpret_cast<Bone*>(bone_dst_ptr);
        src_ptr += sizeof(Bone);
        dst_ptr += sizeof(Bone);
        const Bone::Index children_count = bone.children_count;
        if (children_count > 0)
        {
            const Bone::Index children_indices_count = children_count * sizeof(Bone::Index);
            std::memcpy(&bones[dst_ptr], &armature_bones[src_ptr], static_cast<std::size_t>(children_indices_count));
            src_ptr += children_indices_count;
            dst_ptr += children_indices_count;
        }
    }

    Armature arm;
    arm.root_bone_index = root_bone_ptr;
    arm.name = name;
    builder.add_component(std::move(arm));
}

void gearoenix::physics::animation::Manager::update() noexcept {
    core::sync::ParallelFor::exec(bone_indices.begin(), bone_indices.end(), [this](const Bone::Index bone_index, auto) noexcept {
        reinterpret_cast<Bone*>(&bones[bone_index])->update();
    });
}