#pragma once
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../core/job/gx-cr-job-end-caller.hpp"

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::physics::animation {
struct Bone;
struct Armature final : core::ecs::Component {
    GEAROENIX_OBJECT_STRUCT_DEF;

    constexpr static auto max_count = 8;
    constexpr static auto object_type_index = gearoenix_physics_animation_armature_type_index;

    GX_GET_CREF_PRV(std::shared_ptr<Bone>, root_bone);
    GX_GET_CREF_PRV(std::vector<Bone*>, all_bones);
    GX_GET_PTR_PRV(Transformation, transform);

    Armature(core::ecs::Entity* entity, std::string&& name, Transformation* transform);
    void insert_bone(Bone* bone);

public:
    ~Armature() override;
    void set_root_bone(std::shared_ptr<Bone>&&);
    void sort_all_bones(const std::vector<int>& bone_indices);
    void write(std::shared_ptr<platform::stream::Stream>&& stream, std::shared_ptr<core::ObjectStreamer>&& object_streamer, core::job::EndCaller<>&& end) override;
    void read(platform::stream::Stream&, core::job::EndCaller<>&&);
};
}