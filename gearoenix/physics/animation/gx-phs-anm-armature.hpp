#pragma once
#include "../../core/ecs/gx-cr-ecs-component.hpp"

namespace gearoenix::physics::animation {
struct Manager;
struct Armature final : core::ecs::Component {
    constexpr static std::uint32_t MAX_COUNT = 1;
    constexpr static TypeIndex TYPE_INDEX = 2;
    constexpr static TypeIndexSet ALL_PARENT_TYPE_INDICES {};
    constexpr static TypeIndexSet IMMEDIATE_PARENT_TYPE_INDICES {};

    std::uint32_t root_bone_index = static_cast<std::uint32_t>(-1);

    Armature(std::string&& name, core::ecs::entity_id_t entity_id);
    ~Armature() override;
};
}