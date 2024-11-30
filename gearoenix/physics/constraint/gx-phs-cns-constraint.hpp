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
    constexpr static std::uint32_t MAX_COUNT = 0;
    constexpr static TypeIndex TYPE_INDEX = 6;
    constexpr static TypeIndexSet ALL_PARENT_TYPE_INDICES {};
    constexpr static TypeIndexSet IMMEDIATE_PARENT_TYPE_INDICES {};

    Constraint(TypeIndex final_type_index, std::string&& name, core::ecs::entity_id_t entity_id);
    ~Constraint() override;
    virtual void update();
};
}