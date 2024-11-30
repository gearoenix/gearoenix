#pragma once
#include "../../core/ecs/gx-cr-ecs-types.hpp"
#include "../../core/event/gx-cr-ev-listener.hpp"
#include "gx-rnd-wdg-size.hpp"

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::widget {
struct Layout : core::event::Listener {
    engine::Engine& e;

    explicit Layout(engine::Engine& e)
        : e(e) { };
    ~Layout() override = default;
};

struct LayoutWindowBasedPlacer final : Layout {
    enum struct Side {
        Left,
        Right,
        Bottom,
        Top
    };

    const Size distance;
    const Side side;
    const core::ecs::entity_id_t model_entity_id;

    LayoutWindowBasedPlacer(
        engine::Engine& e, const Size distance, const Side side, const core::ecs::entity_id_t model_entity_id)
        : Layout(e)
        , distance(distance)
        , side(side)
        , model_entity_id(model_entity_id)
    {
    }
    ~LayoutWindowBasedPlacer() override = default;
    void set_location(physics::Transformation&) const;
    [[nodiscard]] Response on_event(const core::event::Data& event_data) override;
};
}