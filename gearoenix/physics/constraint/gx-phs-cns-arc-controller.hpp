#pragma once
#include "../../core/event/gx-cr-ev-listener.hpp"
#include "../../math/gx-math-vector-3d.hpp"
#include <memory>

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::camera {
// struct ArcController final : public core::event::Listener {
//     GX_GETSET_VAL_PRV(core::fp_t, movement_speed, 0.5)
//     GX_GETSET_VAL_PRV(core::fp_t, touch_scale_speed, 1.5)
//     GX_GETSET_VAL_PRV(core::fp_t, rotation_speed, 0.5)
//     GX_GETSET_VAL_PRV(core::fp_t, rotate_z, 0.0)
//     GX_GET_CREF_PRV(math::Vec3<core::fp_t>, target)
//     GX_GET_CREF_PRV(math::Vec3<core::fp_t>, up)
//     GX_GET_CREF_PRV(math::Vec3<core::fp_t>, direction)
//     GX_GET_VAL_PRV(core::fp_t, max_vertical_angle, 1.0)
//     GX_GET_VAL_PRV(core::fp_t, min_vertical_angle, -1.0)
//     GX_GET_VAL_PRV(core::fp_t, vertical_angle, (max_vertical_angle + min_vertical_angle) * 0.5)
//     GX_GET_VAL_PRV(core::fp_t, max_distance, 10.0)
//     GX_GET_VAL_PRV(core::fp_t, min_distance, 1.0)
//     GX_GET_VAL_PRV(core::fp_t, distance, (max_distance + min_distance) * 0.5)
//     GX_GET_REFC_PRV(std::shared_ptr<Camera>, controlled_camera)
//  private:
//     Transformation* const trn;
//     engine::Engine* const render_engine;
//     const core::Id function_id;
//     core::fp_t rotate_x = 0.0;
//     bool move_forward = false;
//     bool move_backward = false;
//     bool rotate_left = false;
//     bool rotate_right = false;
//
//     void update() ;
//     void values_updated() ;
//
//  public:
//     explicit ArcController(std::shared_ptr<Camera> cam) ;
//     ~ArcController()  override;
//     void set_vertical_angle(core::fp_t) ;
//     void set_min_vertical_angle(core::fp_t) ;
//     void set_max_vertical_angle(core::fp_t) ;
//     void set_distance(core::fp_t) ;
//     void set_min_distance(core::fp_t) ;
//     void set_max_distance(core::fp_t) ;
//     void set_target(const math::Vec3<core::fp_t>&) ;
//     [[nodiscard]] bool on_event(const core::event::Data& event_data)  override;
//};
}