#ifndef GEAROENIX_RENDER_SCENE_MANAGER_HPP
#define GEAROENIX_RENDER_SCENE_MANAGER_HPP
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include <memory>
#include <string>
#include <vector>

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::scene {
struct Builder;
struct Manager final {
private:
    engine::Engine& e;

public:
    explicit Manager(engine::Engine& e) noexcept;
    ~Manager() noexcept;
    Manager(Manager&&) = delete;
    Manager(const Manager&) = delete;
    /// By layer you decide in what order scenes to be render on top of each other
    [[nodiscard]] std::shared_ptr<Builder> build(const std::string& name, double layer = 0.0) noexcept;
    void update() noexcept;
};
}
#endif
