#pragma once
#include "../../core/ecs/gx-cr-ecs-entity-ptr.hpp"
#include "../../core/gx-cr-singleton.hpp"
#include <string>

namespace gearoenix::render::scene {
struct Manager : core::Singleton<Manager> {
protected:
    Manager();

public:
    ~Manager() override;

    /// By layer, you decide in what order scenes to be rendered on top of each other
    [[nodiscard]] virtual core::ecs::EntityPtr build(std::string&& name, double layer) const;
    virtual void update();
};
}
