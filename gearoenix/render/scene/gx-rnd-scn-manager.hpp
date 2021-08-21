#ifndef GEAROENIX_RENDER_SCENE_MANAGER_HPP
#define GEAROENIX_RENDER_SCENE_MANAGER_HPP
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include <memory>
#include <string>
#include <vector>

namespace gearoenix::core::sync {
struct WorkWaiter;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::scene {
struct Builder;
struct Manager final {
private:
    engine::Engine& e;
    std::unique_ptr<core::sync::WorkWaiter> io_worker;
    std::vector<std::pair<std::string, core::ecs::Entity::id_t>> name_to_id;
    std::vector<std::pair<core::ecs::Entity::id_t, std::string>> id_to_name;
    std::vector<std::pair<float, core::ecs::Entity::id_t>> render_layers;

public:
    constexpr static const float DEFAULT_RENDER_LAYER = 0.0f;

    explicit Manager(engine::Engine& e) noexcept;
    ~Manager() noexcept;
    [[nodiscard]] std::shared_ptr<Builder> build() noexcept;
};
}
#endif
