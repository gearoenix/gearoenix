#pragma once
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include <memory>
#include <string>

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::scene {
struct Builder;
struct Manager final {
private:
    engine::Engine& e;

public:
    explicit Manager(engine::Engine& e);
    ~Manager();
    Manager(Manager&&) = delete;
    Manager(const Manager&) = delete;
    /// By layer, you decide in what order scenes to be rendered on top of each other
    [[nodiscard]] std::shared_ptr<Builder> build(const std::string& name, double layer, core::job::EndCaller<>&& end_callback) const;
    void update() const;
};
}
