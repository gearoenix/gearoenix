#include "gx-phs-anm-bone.hpp"

gearoenix::physics::animation::BoneInfo::BoneInfo(std::string&& name)
    : transform(nullptr)
    , name(std::move(name))
{
}

gearoenix::physics::animation::Bone::Bone(
    std::shared_ptr<Transformation>&& transform,
    math::Mat4x4<float>&& inverse_bind,
    std::string&& name,
    const std::uint32_t parent_index,
    const std::uint32_t children_count,
    const std::uint32_t first_child_index,
    const std::uint32_t last_child_index)
    : transform(std::move(transform))
    , inverse_bind(inverse_bind)
    , name(std::move(name))
    , parent_index(parent_index)
    , children_count(children_count)
    , first_child_index(first_child_index)
    , end_child_index(last_child_index)
{
}
