#ifndef GEAROENIX_RENDER_CAMERA_JET_CONTROLLER_HPP
#define GEAROENIX_RENDER_CAMERA_JET_CONTROLLER_HPP
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../core/event/gx-cr-ev-listener.hpp"

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::camera {
struct JetController final : public core::event::Listener {
    GX_GETSET_VAL_PRV(core::ecs::entity_id_t, camera_entity_id, 0);
    GX_GETSET_VAL_PRV(double, movement_speed, 0.5);
    GX_GETSET_VAL_PRV(double, rotation_speed, 0.5);

private:
    engine::Engine& e;
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

    [[nodiscard]] Response on_event(const core::event::Data& event_data) final;

public:
    JetController(engine::Engine& e, core::ecs::entity_id_t camera_entity_id);
    ~JetController() final;
    void update();
};
}
#endif
