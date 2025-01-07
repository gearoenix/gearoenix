#include "gx-phs-anm-armature.hpp"
#include "../../core/ecs/gx-cr-ecs-comp-allocator.hpp"
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../gx-phs-transformation.hpp"
#include "gx-phs-anm-bone.hpp"

void gearoenix::physics::animation::Armature::insert_bone(Bone* const bone)
{
    const auto index = static_cast<std::uint32_t>(all_bones.size());
    all_bones.emplace_back(bone);
    bone->set_index_in_armature(index);
    for (auto* const child : bone->get_children()) {
        insert_bone(child);
    }
}

void gearoenix::physics::animation::Armature::write_in_io_context(
    std::shared_ptr<platform::stream::Stream>&& stream,
    core::job::EndCaller<>&&) const
{
    stream->write_fail_debug(root_bone->get_entity_id());
    stream->write_fail_debug(static_cast<std::uint32_t>(all_bones.size()));
    for (const auto* const bone : all_bones) {
        stream->write_fail_debug(bone->get_entity_id());
    }
}

void gearoenix::physics::animation::Armature::update_in_io_context(
    std::shared_ptr<platform::stream::Stream>&& stream,
    core::job::EndCaller<>&& end)
{
    const auto rbi = stream->read<core::ecs::entity_id_t>();
    std::vector<core::ecs::entity_id_t> abi(stream->read<std::uint32_t>());
    for (auto& all_bone_id : abi) {
        stream->read(all_bone_id);
    }
    root_bone = nullptr;
    all_bones.clear();
    core::ecs::World::get()->resolve([this, self = component_self.lock(), rbi, abi = std::move(abi), e = std::move(end)]() mutable -> bool {
        (void)self;
        (void)e;
        for (const auto& bone_id : abi) {
            if (nullptr == core::ecs::World::get()->get_component<Bone>(bone_id)) {
                return true;
            }
        }
        root_bone = core::ecs::World::get()->get_component_shared_ptr<Bone>(rbi);
        for (const auto& bone_id : abi) {
            all_bones.push_back(core::ecs::World::get()->get_component<Bone>(bone_id));
        }
        return false;
    });
}

gearoenix::physics::animation::Armature::Armature(
    std::string&& name, std::shared_ptr<Transformation>&& transform, const core::ecs::entity_id_t entity_id)
    : Component(core::ecs::ComponentType::create_index(this), std::move(name), entity_id)
    , transform(std::move(transform))
{
}

gearoenix::physics::animation::Armature::~Armature() = default;

void gearoenix::physics::animation::Armature::set_root_bone(std::shared_ptr<Bone>&& rb)
{
    root_bone = std::move(rb);
    all_bones.clear();
    insert_bone(root_bone.get());
}

void gearoenix::physics::animation::Armature::sort_all_bones(const std::vector<int>& bone_indices)
{
    auto sorted_bones = all_bones;
    for (const auto index : bone_indices) {
        sorted_bones[index] = all_bones[index];
    }
    all_bones = std::move(sorted_bones);
}
