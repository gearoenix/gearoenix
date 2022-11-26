#ifndef GEAROENIX_RENDER_WIDGET_LAYOUT_HPP
#define GEAROENIX_RENDER_WIDGET_LAYOUT_HPP
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
struct Layout : public core::event::Listener {
    engine::Engine& e;

public:
    explicit Layout(engine::Engine& e) noexcept
        : e(e) {};
    ~Layout() noexcept override = default;
};

struct LayoutWindowBasedPlacer final : public Layout {
    enum struct Side {
        Left,
        Right,
        Bottom,
        Top
    };

    const Size distance;
    const Side side;
    const core::ecs::entity_id_t model_entity_id;

    LayoutWindowBasedPlacer(engine::Engine& e, Size distance, Side side, core::ecs::entity_id_t model_entity_id) noexcept
        : Layout(e)
        , distance(distance)
        , side(side)
        , model_entity_id(model_entity_id)
    {
    }
    ~LayoutWindowBasedPlacer() noexcept final = default;
    void set_location(physics::Transformation&) const noexcept;
    [[nodiscard]] core::event::Listener::Response on_event(const core::event::Data& event_data) noexcept final;
};
}
#endif
