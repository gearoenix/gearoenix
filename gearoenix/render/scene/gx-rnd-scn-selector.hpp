#ifndef GEAROENIX_RENDER_SCENE_SELECTOR_HPP
#define GEAROENIX_RENDER_SCENE_SELECTOR_HPP
#include "../../core/ecs/gx-cr-ecs-entity.hpp"

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::scene {
struct Selector final {
    engine::Engine& e;

    GX_GET_CREF_PRV(std::vector<const char*>, entity_names);
    GX_GET_CREF_PRV(std::vector<core::ecs::entity_id_t>, ids);
    std::size_t last_update = 0;
    int current_selection = -1;

public:
    explicit Selector(engine::Engine& e);
    ~Selector();

    Selector(Selector&&) = delete;
    Selector(const Selector&) = delete;

    void show();
    [[nodiscard]] core::ecs::entity_id_t get_selection() const;
};
}
#endif
