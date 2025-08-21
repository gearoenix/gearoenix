#include "gx-phs-anm-armature.hpp"
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "gx-phs-anm-bone.hpp"

void gearoenix::physics::animation::Armature::insert_bone(Bone* const bone)
{
    const auto index = static_cast<std::uint32_t>(all_bones.size());
    all_bones.emplace_back(bone);
    bone->set_index_in_armature(index);
    for (const auto& child : bone->get_entity()->get_children()) {
        if (auto* const c = child.second->get_component<Bone>(); c) {
            insert_bone(c);
        }
    }
}

void gearoenix::physics::animation::Armature::write(platform::stream::Stream& stream) const
{
    GX_TODO; // I have to use an object streamer
    stream.write_fail_debug(root_bone->get_object_id());
    stream.write_fail_debug(static_cast<std::uint32_t>(all_bones.size()));
    for (const auto* const bone : all_bones) {
        stream.write_fail_debug(bone->get_object_id());
    }
}

void gearoenix::physics::animation::Armature::read(platform::stream::Stream& stream, core::job::EndCaller<>&& end)
{
    GX_TODO; // I have to use
    const auto rbi = stream.read<core::object_id_t>();
    std::vector<core::object_id_t> abi(stream.read<std::uint32_t>());
    for (auto& all_bone_id : abi) {
        stream.read(all_bone_id);
    }
    root_bone = nullptr;
    all_bones.clear();
    core::ecs::World::get().resolve([this, self = object_self.lock(), rbi, abi = std::move(abi), e = std::move(end)]() mutable -> bool {
        (void)self;
        (void)e;
        for (const auto bone_id : abi) {
            if (nullptr == find_object(bone_id)) {
                return true;
            }
        }
        root_bone = std::move(find_object_t<Bone>(rbi));
        GX_ASSERT_D(nullptr != root_bone);
        for (const auto bone_id : abi) {
            auto* const bone = find_object_t<Bone>(bone_id).get();
            GX_ASSERT_D(nullptr != bone);
            all_bones.push_back(bone);
        }
        return false;
    });
}

gearoenix::physics::animation::Armature::Armature(core::ecs::Entity* const entity, std::string&& name, Transformation* transform)
    : Component(entity, core::ecs::ComponentType::create_index(this), std::move(name))
    , transform(transform)
{
}

gearoenix::physics::animation::Armature::~Armature() = default;

void gearoenix::physics::animation::Armature::set_root_bone(std::shared_ptr<Bone>&& b)
{
    root_bone = std::move(b);
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
