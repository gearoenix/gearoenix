#pragma once
#include "../../core/event/gx-cr-ev-listener.hpp"
#include "gx-phs-cns-constraint.hpp"

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::physics::constraint {
struct JetController final : core::event::Listener, Constraint {
    GEAROENIX_OBJECT_STRUCT_DEF;

    constexpr static auto max_count = 8;
    constexpr static auto object_type_index = gearoenix_physics_constraint_jet_controller_type_index;
    constexpr static std::array all_parent_object_type_indices { Constraint::object_type_index };
    constexpr static std::array immediate_parent_object_type_indices { Constraint::object_type_index };

    GX_GETSET_VAL_PRV(double, movement_speed, 10.0);
    GX_GETSET_VAL_PRV(double, rotation_speed, 0.5);

    GX_GETSET_CREF_PRV(std::shared_ptr<Transformation>, transformation);

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

    JetController(core::ecs::Entity* entity, std::shared_ptr<Transformation>&&, std::string&& name);

public:
    ~JetController() override;
    void update() override;
    void clear_transforms();
};
}