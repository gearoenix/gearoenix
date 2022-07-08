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
    std::string name;
    std::string parent_name;
    std::vector<BoneInfo> children;
};

struct Bone final {
    Transformation transform;
    std::string name;
    std::size_t parent_index = static_cast<std::size_t>(-1);
    std::size_t children_count = 0;
    std::size_t first_child_index = static_cast<std::size_t>(-1);
    std::size_t end_child_index = static_cast<std::size_t>(-1);

    Bone(
        Transformation&& transform,
        std::string&& name,
        std::size_t parent_index,
        std::size_t children_count,
        std::size_t first_child_index,
        std::size_t last_child_index) noexcept;
};
}
#endif