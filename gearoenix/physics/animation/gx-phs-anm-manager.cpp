#include "gx-phs-anm-manager.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../core/sync/gx-cr-sync-parallel-for.hpp"
#include "../../render/engine/gx-rnd-eng-engine.hpp"
#include "gx-phs-anm-armature.hpp"
#include "gx-phs-anm-bone.hpp"

gearoenix::physics::animation::Manager::Manager(render::engine::Engine& e) noexcept
    : e(e)
{
}

gearoenix::physics::animation::Manager::~Manager() noexcept = default;

void gearoenix::physics::animation::Manager::create_armature(
    core::ecs::EntityBuilder& builder,
    const std::string& name,
    const std::vector<std::uint8_t>& armature_bones) noexcept
{
    GX_GUARD_LOCK(bones);

    const auto root_bone_index = static_cast<Bone::Index>(bones.size());

    Armature arm;
    arm.root_bone_index = root_bone_index;
    arm.name = name;
    builder.add_component(std::move(arm));

    bones.resize(bones.size() + armature_bones.size());
    std::memcpy(&bones[root_bone_index], armature_bones.data(), armature_bones.size());

    const auto last_bone_index = static_cast<Bone::Index>(bones.size());

    for (Bone::Index bone_index = root_bone_index; bone_index < last_bone_index; bone_index += sizeof(Bone)) {
        bone_indices.push_back(bone_index);
        bone_index += reinterpret_cast<const Bone*>(&bones[bone_index])->children_count * sizeof(Bone::Index);
    }
}

void gearoenix::physics::animation::Manager::update() noexcept
{
    core::sync::ParallelFor::exec(bone_indices.begin(), bone_indices.end(), [this](const Bone::Index bone_index, auto) noexcept {
        reinterpret_cast<Bone*>(&bones[bone_index])->update();
    });
}