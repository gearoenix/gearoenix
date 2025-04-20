#pragma once
#include "../../core/ecs/gx-cr-ecs-entity-ptr.hpp"
#include <string>

namespace gearoenix::render::scene {
struct Manager {
protected:
    Manager();

public:
    virtual ~Manager();
    Manager(Manager&&) = delete;
    Manager(const Manager&) = delete;
    [[nodiscard]] static Manager& get();

    /// By layer, you decide in what order scenes to be rendered on top of each other
    [[nodiscard]] virtual core::ecs::EntityPtr build(std::string&& name, double layer) const;
    virtual void update() const;
};
}
