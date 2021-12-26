#ifndef GEAROENIX_RENDER_CAMERA_ARC_CONTROLLER_HPP
#define GEAROENIX_RENDER_CAMERA_ARC_CONTROLLER_HPP
#include "../../core/event/gx-cr-ev-listener.hpp"
#include "../../math/gx-math-vector-3d.hpp"
#include <memory>

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::camera {
struct Camera;
struct Transformation;
struct ArcController final : public core::event::Listener {
    GX_GETSET_VAL_PRV(double, movement_speed, 0.5)
    GX_GETSET_VAL_PRV(double, touch_scale_speed, 1.5)
    GX_GETSET_VAL_PRV(double, rotation_speed, 0.5)
    GX_GETSET_VAL_PRV(double, rotate_z, 0.0)
    GX_GET_CREF_PRV(math::Vec3<double>, target)
    GX_GET_CREF_PRV(math::Vec3<double>, up)
    GX_GET_CREF_PRV(math::Vec3<double>, direction)
    GX_GET_VAL_PRV(double, max_vertical_angle, 1.0)
    GX_GET_VAL_PRV(double, min_vertical_angle, -1.0)
    GX_GET_VAL_PRV(double, vertical_angle, (max_vertical_angle + min_vertical_angle) * 0.5)
    GX_GET_VAL_PRV(double, max_distance, 10.0)
    GX_GET_VAL_PRV(double, min_distance, 1.0)
    GX_GET_VAL_PRV(double, distance, (max_distance + min_distance) * 0.5)
    GX_GET_REFC_PRV(std::shared_ptr<Camera>, controlled_camera)
private:
    Transformation* const trn;
    engine::Engine* const render_engine;
    const core::Id function_id;
    double rotate_x = 0.0;
    bool move_forward = false;
    bool move_backward = false;
    bool rotate_left = false;
    bool rotate_right = false;

    void update() noexcept;
    void values_updated() noexcept;

public:
    explicit ArcController(std::shared_ptr<Camera> cam) noexcept;
    ~ArcController() noexcept final;
    void set_vertical_angle(double) noexcept;
    void set_min_vertical_angle(double) noexcept;
    void set_max_vertical_angle(double) noexcept;
    void set_distance(double) noexcept;
    void set_min_distance(double) noexcept;
    void set_max_distance(double) noexcept;
    void set_target(const math::Vec3<double>&) noexcept;
    [[nodiscard]] bool on_event(const core::event::Data& event_data) noexcept final;
};
}
#endif
