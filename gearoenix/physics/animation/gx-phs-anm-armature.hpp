#ifndef GEAROENIX_PHYSICS_ANIMATION_ARMATURE_HPP
#define GEAROENIX_PHYSICS_ANIMATION_ARMATURE_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include <string>

namespace gearoenix::physics::animation {
struct Manager;
struct Armature final : core::ecs::Component {
    std::string name;
    std::uint32_t root_bone_index = static_cast<std::uint32_t>(-1);

    Armature() noexcept;
    Armature(Armature&&) noexcept;
    ~Armature() noexcept final;
};
}
#endif