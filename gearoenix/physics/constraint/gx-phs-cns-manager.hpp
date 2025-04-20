#pragma once
#include "../../core/ecs/gx-cr-ecs-entity-ptr.hpp"
#include <memory>
#include <string>

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::physics::constraint {
struct Constraint;
struct Manager final {
    Manager();
    ~Manager();
    [[nodiscard]] core::ecs::EntityPtr create_jet_controller(
        std::string&& name, std::shared_ptr<Transformation>&& transform, core::ecs::Entity* parent) const;
    void update();
};
}