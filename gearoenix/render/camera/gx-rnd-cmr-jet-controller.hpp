#ifndef GEAROENIX_RENDER_CAMERA_JET_CONTROLLER_HPP
#define GEAROENIX_RENDER_CAMERA_JET_CONTROLLER_HPP
#include "../../core/event/gx-cr-ev-listener.hpp"
#include <memory>

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::camera {
struct Camera;
struct Transformation;
struct JetController final : public core::event::Listener {
private:
    Transformation* const trn;
    engine::Engine* const render_engine;
    const std::shared_ptr<Camera> cam;
    const core::Id function_id;
    double rotate_x = 0.0;
    double rotate_z = 0.0;
    double movement_speed = 0.5;
    double rotation_speed = 0.5;
    bool move_forward = false;
    bool move_rightward = false;
    bool move_leftward = false;
    bool move_backward = false;
    bool rotate_left = false;
    bool rotate_right = false;

    void update() noexcept;

public:
    explicit JetController(std::shared_ptr<Camera> cam) noexcept;
    ~JetController() noexcept final;
    [[nodiscard]] bool on_event(const core::event::Data& event_data) noexcept final;
};
}
#endif
