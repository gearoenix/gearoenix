#pragma once
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../core/ecs/gx-cr-ecs-entity-ptr.hpp"
#include "../../core/gx-cr-static-flat-set.hpp"
#include "gx-phs-anm-keyframe.hpp"

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::physics::animation {
struct Manager;
typedef boost::container::flat_map<double, Keyframe<math::Vec3<double>>> Vec3KeyFrameMap;
typedef boost::container::flat_map<double, Keyframe<math::Quat<double>>> QuatKeyFrameMap;
struct Bone final : core::ecs::Component {
    constexpr static auto max_bone_children_count = 8;
    typedef core::static_flat_set<Bone*, max_bone_children_count> bone_children_t;

    constexpr static auto max_count = 1024;
    constexpr static auto object_type_index = gearoenix_physics_animation_bone_type_index;

    GX_GET_CREF_PRV(std::shared_ptr<Transformation>, transform);
    GX_GET_CREF_PRV(math::Mat4x4<double>, global_matrix);
    GX_GET_CREF_PRV(math::Mat4x4<double>, inverted_global_matrix);
    GX_GET_CREF_PRV(math::Mat4x4<double>, inverse_bind_matrix);
    GX_GET_CREF_PRV(math::Mat4x4<double>, bind_matrix);
    GX_GET_REF_PRV(Vec3KeyFrameMap, scale_samples);
    GX_GET_REF_PRV(QuatKeyFrameMap, rotation_samples);
    GX_GET_REF_PRV(Vec3KeyFrameMap, translation_samples);
    GX_GETSET_VAL_PRV(std::uint32_t, index_in_armature, static_cast<std::uint32_t>(-1));

    void write(platform::stream::Stream&) const override;
    void read(platform::stream::Stream&, core::job::EndCaller<>&&);

public:
    Bone(std::shared_ptr<Transformation>&& transform, std::string&& name);
    ~Bone() override;
    [[nodiscard]] static core::ecs::EntityPtr build(std::string&& name, core::ecs::Entity* parent);
    void set_inverse_bind_matrix(const math::Mat4x4<double>& m);
    void update_matrices();
    static void update_all_bones_after_transform_updates();
};
}