#pragma once
#include "../../core/ecs/gx-cr-ecs-entity-ptr.hpp"
#include "../../core/gx-cr-singleton.hpp"
#include <memory>
#include <string>

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::physics::constraint {
struct Constraint;
struct Manager final : core::Singleton<Manager> {
    Manager();
    ~Manager() override;
    [[nodiscard]] core::ecs::EntityPtr create_jet_controller(
        std::string&& name, std::shared_ptr<Transformation>&& transform, core::ecs::Entity* parent) const;
    void update();
};
}