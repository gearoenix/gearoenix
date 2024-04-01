#ifndef GEAROENIX_PHYSICS_ANIMATION_BONE_HPP
#define GEAROENIX_PHYSICS_ANIMATION_BONE_HPP
#include "../gx-phs-transformation.hpp"
#include <string>
#include <vector>

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::physics::animation {
struct Manager;

struct BoneInfo final {
    Transformation transform;
    math::Mat4x4<float> inverse_bind;
    std::string parent_name;
    std::vector<BoneInfo> children;
    std::string name;

    explicit BoneInfo(std::string&& name);
};

struct Bone final {
    Transformation transform;
    math::Mat4x4<float> inverse_bind;
    math::Mat4x4<float> m;
    math::Mat4x4<float> inv_m;
    std::string name;
    std::size_t parent_index = static_cast<std::size_t>(-1);
    std::size_t children_count = 0;
    std::size_t first_child_index = static_cast<std::size_t>(-1);
    std::size_t end_child_index = static_cast<std::size_t>(-1);

    Bone(
        Transformation&& transform,
        math::Mat4x4<float>&& inverse_bind,
        std::string&& name,
        std::size_t parent_index,
        std::size_t children_count,
        std::size_t first_child_index,
        std::size_t last_child_index);
};
}
#endif