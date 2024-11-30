#pragma once
#include "../gx-phs-transformation.hpp"
#include <string>
#include <vector>

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::physics::animation {
struct Manager;

struct BoneInfo final {
    std::shared_ptr<Transformation> transform;
    math::Mat4x4<float> inverse_bind;
    std::string parent_name;
    std::vector<BoneInfo> children;
    std::string name;

    explicit BoneInfo(std::string&& name);
};

struct Bone final {
    std::shared_ptr<Transformation> transform;
    math::Mat4x4<float> inverse_bind; // TODO: this should remove if we make bones as entity
    math::Mat4x4<float> m; // TODO: this should remove if we make bones as entity
    math::Mat4x4<float> inv_m; // TODO: this should remove if we make bones as entity
    std::string name;
    std::uint32_t parent_index = static_cast<std::uint32_t>(-1);
    std::uint32_t children_count = 0;
    std::uint32_t first_child_index = static_cast<std::uint32_t>(-1);
    std::uint32_t end_child_index = static_cast<std::uint32_t>(-1);

    Bone(
        std::shared_ptr<Transformation>&& transform,
        math::Mat4x4<float>&& inverse_bind,
        std::string&& name,
        std::uint32_t parent_index,
        std::uint32_t children_count,
        std::uint32_t first_child_index,
        std::uint32_t last_child_index);
};
}