#pragma once
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../core/job/gx-cr-job-end-caller.hpp"

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::physics::animation {
struct Bone;
struct Armature final : core::ecs::Component {
    constexpr static auto max_count = 8;
    constexpr static auto object_type_index = gearoenix_physics_animation_armature_type_index;

    GX_GET_CREF_PRV(std::shared_ptr<Bone>, root_bone);
    GX_GET_CREF_PRV(std::vector<Bone*>, all_bones);
    GX_GET_PTR_PRV(Transformation, transform);

    void insert_bone(Bone* bone);

public:
    Armature(std::string&& name, Transformation* transform);
    ~Armature() override;
    void set_root_bone(std::shared_ptr<Bone>&&);
    void sort_all_bones(const std::vector<int>& bone_indices);
    void write(platform::stream::Stream&) const override;
    void read(platform::stream::Stream&, core::job::EndCaller<>&&);
};
}