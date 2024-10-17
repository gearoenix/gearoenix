#ifndef GEAROENIX_PHYSICS_CONSTRAINT_CONSTRAINT_HPP
#define GEAROENIX_PHYSICS_CONSTRAINT_CONSTRAINT_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::physics::constraint {
struct Constraint : core::ecs::Component {
    Constraint(std::type_index final_type_index, std::string&& name, core::ecs::entity_id_t entity_id);
    ~Constraint() override;
    virtual void update();
};
}
#endif
