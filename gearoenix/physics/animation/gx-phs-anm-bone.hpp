#pragma once
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "gx-phs-anm-keyframe.hpp"
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::physics::animation {
struct Manager;
typedef boost::container::flat_map<double, Keyframe<math::Vec3<double>>> Vec3KeyFrameMap;
typedef boost::container::flat_map<double, Keyframe<math::Quat<double>>> QuatKeyFrameMap;
struct Bone final : core::ecs::Component {
    constexpr static std::uint32_t MAX_COUNT = 1024;
    constexpr static core::ecs::component_index_t TYPE_INDEX = 29;
    constexpr static core::ecs::component_index_set_t ALL_PARENT_TYPE_INDICES {};
    constexpr static core::ecs::component_index_set_t IMMEDIATE_PARENT_TYPE_INDICES {};

    GX_GET_CREF_PRV(std::shared_ptr<Transformation>, transform);
    GX_GET_CREF_PRV(math::Mat4x4<double>, global_matrix);
    GX_GET_CREF_PRV(math::Mat4x4<double>, inverted_global_matrix);
    GX_GET_CREF_PRV(math::Mat4x4<double>, inverse_bind_matrix);
    GX_GET_CREF_PRV(math::Mat4x4<double>, bind_matrix);
    GX_GET_REF_PRV(Vec3KeyFrameMap, scale_samples);
    GX_GET_REF_PRV(QuatKeyFrameMap, rotation_samples);
    GX_GET_REF_PRV(Vec3KeyFrameMap, translation_samples);
    GX_GET_PTR_PRV(Bone, parent);
    GX_GET_CREF_PRV(boost::container::flat_set<Bone*>, children);
    GX_GETSET_VAL_PRV(std::uint32_t, index_in_armature, static_cast<std::uint32_t>(-1));

    void write_in_io_context(std::shared_ptr<platform::stream::Stream>&&, core::job::EndCaller<>&&) const override;
    void update_in_io_context(std::shared_ptr<platform::stream::Stream>&&, core::job::EndCaller<>&&) override;

public:
    Bone(
        Bone* parent,
        std::shared_ptr<Transformation>&& transform,
        std::string&& name,
        core::ecs::entity_id_t entity_id);
    ~Bone() override;
    void add_child(Bone* child);
    void set_inverse_bind_matrix(const math::Mat4x4<double>& m);
    void update_matrices();
    static void update_all_bones_after_transform_updates();
};
}