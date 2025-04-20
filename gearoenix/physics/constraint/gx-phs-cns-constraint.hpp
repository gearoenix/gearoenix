#pragma once
#include "../../core/ecs/gx-cr-ecs-component.hpp"

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::physics::constraint {
struct Constraint : core::ecs::Component {
    constexpr static auto max_count = 4;
    constexpr static auto object_type_index = gearoenix_physics_constraint_type_index;

    Constraint(core::object_type_index_t final_type_index, std::string&& name);
    ~Constraint() override;
    virtual void update();
};
}