#pragma once
#include "../../core/ecs/gx-cr-ecs-component.hpp"

namespace gearoenix::physics::animation {
struct Manager;
struct Armature final : core::ecs::Component {
    constexpr static std::uint32_t MAX_COUNT = 1;
    constexpr static core::ecs::component_index_t TYPE_INDEX = 2;
    constexpr static core::ecs::component_index_set_t ALL_PARENT_TYPE_INDICES {};
    constexpr static core::ecs::component_index_set_t IMMEDIATE_PARENT_TYPE_INDICES {};

    std::uint32_t root_bone_index = static_cast<std::uint32_t>(-1);

    Armature(std::string&& name, core::ecs::entity_id_t entity_id);
    ~Armature() override;
};
}