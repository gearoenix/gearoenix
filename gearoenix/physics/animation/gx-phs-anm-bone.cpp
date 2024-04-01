#include "gx-phs-anm-bone.hpp"

gearoenix::physics::animation::BoneInfo::BoneInfo(std::string&& name)
    : name(std::move(name))
{
}

gearoenix::physics::animation::Bone::Bone(
    Transformation&& transform,
    math::Mat4x4<float>&& inverse_bind,
    std::string&& name,
    const std::size_t parent_index,
    const std::size_t children_count,
    const std::size_t first_child_index,
    const std::size_t last_child_index)
    : transform(std::move(transform))
    , inverse_bind(inverse_bind)
    , name(std::move(name))
    , parent_index(parent_index)
    , children_count(children_count)
    , first_child_index(first_child_index)
    , end_child_index(last_child_index)
{
}
