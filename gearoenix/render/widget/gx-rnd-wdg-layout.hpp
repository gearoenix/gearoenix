#pragma once
#include "../../core/event/gx-cr-ev-listener.hpp"
#include "gx-rnd-wdg-size.hpp"

namespace gearoenix::core::ecs {
struct Entity;
}

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::render::widget {
struct Layout : core::event::Listener {
    Layout() { }
    ~Layout() override = default;
};

struct LayoutWindowBasedPlacer final : Layout {
    enum struct Side { Left,
        Right,
        Bottom,
        Top };

    const Size distance;
    const Side side;
    core::ecs::Entity* const model_entity;

    LayoutWindowBasedPlacer(const Size distance, const Side side, core::ecs::Entity* const model_entity)
        : distance(distance)
        , side(side)
        , model_entity(model_entity)
    {
    }
    ~LayoutWindowBasedPlacer() override = default;
    void set_location(physics::Transformation&) const;
    [[nodiscard]] Response on_event(const core::event::Data& event_data) override;
};
}