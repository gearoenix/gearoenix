#ifndef GEAROENIX_PHYSICS_ANIMATION_ARMATURE_HPP
#define GEAROENIX_PHYSICS_ANIMATION_ARMATURE_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"

namespace gearoenix::physics::animation {
struct Manager;
struct Armature final : core::ecs::Component {
    std::size_t root_bone_index = static_cast<std::size_t>(-1);
    std::size_t bones_count = static_cast<std::size_t>(-1);

    Armature() noexcept;
    Armature(Armature&&) noexcept;
    ~Armature() noexcept final;
};
}
#endif