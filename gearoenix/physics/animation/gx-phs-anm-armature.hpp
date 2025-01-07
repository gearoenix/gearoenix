#pragma once
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../core/job/gx-cr-job-end-caller.hpp"

namespace gearoenix::core::ecs {
struct EntitySharedBuilder;
}

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::physics::animation {
struct Bone;
struct Armature final : core::ecs::Component {
    constexpr static std::uint32_t MAX_COUNT = 8;
    constexpr static core::ecs::component_index_t TYPE_INDEX = 2;
    constexpr static core::ecs::component_index_set_t ALL_PARENT_TYPE_INDICES {};
    constexpr static core::ecs::component_index_set_t IMMEDIATE_PARENT_TYPE_INDICES {};

    GX_GET_CREF_PRV(std::shared_ptr<Bone>, root_bone);
    GX_GET_CREF_PRV(std::vector<Bone*>, all_bones);
    GX_GET_CREF_PRV(std::shared_ptr<Transformation>, transform);
    GX_GETSET_VAL_PRV(core::ecs::entity_id_t, scene_id, 0);

    void insert_bone(Bone* bone);
    void write_in_io_context(std::shared_ptr<platform::stream::Stream>&&, core::job::EndCaller<>&&) const override;
    void update_in_io_context(std::shared_ptr<platform::stream::Stream>&&, core::job::EndCaller<>&&) override;

public:
    Armature(std::string&& name, std::shared_ptr<Transformation>&& transform, core::ecs::entity_id_t entity_id);
    ~Armature() override;
    void set_root_bone(std::shared_ptr<Bone>&&);
    void sort_all_bones(const std::vector<int>& bone_indices);
};
}