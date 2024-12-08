#pragma once
#include "../../core/ecs/gx-cr-ecs-component.hpp"

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::physics::constraint {
struct Constraint : core::ecs::Component {
    constexpr static std::uint32_t MAX_COUNT = 4;
    constexpr static core::ecs::component_index_t TYPE_INDEX = 6;
    constexpr static core::ecs::component_index_set_t ALL_PARENT_TYPE_INDICES {};
    constexpr static core::ecs::component_index_set_t IMMEDIATE_PARENT_TYPE_INDICES {};

    Constraint(core::ecs::component_index_t final_type_index, std::string&& name, core::ecs::entity_id_t entity_id);
    ~Constraint() override;
    virtual void update();
};
}