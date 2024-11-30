#pragma once
#include "../../core/event/gx-cr-ev-listener.hpp"
#include "gx-phs-cns-constraint.hpp"

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::physics::constraint {
struct JetController final : core::event::Listener, Constraint {
    constexpr static std::uint32_t MAX_COUNT = 8;
    constexpr static TypeIndex TYPE_INDEX = 7;
    constexpr static TypeIndexSet ALL_PARENT_TYPE_INDICES { Constraint::TYPE_INDEX };
    constexpr static TypeIndexSet IMMEDIATE_PARENT_TYPE_INDICES { Constraint::TYPE_INDEX };

    GX_GETSET_VAL_PRV(double, movement_speed, 10.0);
    GX_GETSET_VAL_PRV(double, rotation_speed, 0.5);

    std::shared_ptr<Transformation> transformation;
    render::engine::Engine& e;
    double rotate_x = 0.0;
    double rotate_z = 0.0;
    double move_forward_accumulated = 0.0;
    bool move_forward = false;
    bool move_rightward = false;
    bool move_leftward = false;
    bool move_backward = false;
    bool rotate_left = false;
    bool rotate_right = false;
    bool rotate = false;

    [[nodiscard]] Response on_event(const core::event::Data& event_data) override;

public:
    JetController(
        render::engine::Engine& e,
        std::shared_ptr<Transformation>&&,
        std::string&& name,
        core::ecs::entity_id_t entity_id);
    ~JetController() override;
    void update() override;
    void clear_transforms();
};
}