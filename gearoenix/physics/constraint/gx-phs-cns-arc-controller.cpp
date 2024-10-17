#include "gx-phs-cns-arc-controller.hpp"
// #include "../../core/event/gx-cr-ev-engine.hpp"
// #include "../../core/sync/gx-cr-sync-update-manager.hpp"
// #include "../../platform/gx-plt-application.hpp"
// #include "../engine/gx-rnd-eng-engine.hpp"
// #include "gx-rnd-cmr-camera.hpp"

// void gearoenix::render::camera::ArcController::update()
//{
//     bool values_changed = false;
//     if (rotate_x != 0.0) {
//         vertical_angle += rotate_x;
//         rotate_x = 0.0;
//         values_changed = true;
//     }
//     if (rotate_z != 0.0) {
//         trn->global_rotate(rotate_z, up, target);
//         direction = trn->get_z_axis();
//         rotate_z = 0.0;
//     }
//     const auto delta_movement = movement_speed * render_engine->get_delta_time();
//     const auto delta_rotation = rotation_speed * render_engine->get_delta_time();
//     if (move_forward) {
//         distance -= delta_movement;
//         values_changed = true;
//     }
//     if (move_backward) {
//         distance += delta_movement;
//         values_changed = true;
//     }
//     if (rotate_right) {
//         trn->global_rotate(-delta_rotation, up, target);
//         direction = trn->get_z_axis();
//     }
//     if (rotate_left) {
//         trn->global_rotate(delta_rotation, up, target);
//         direction = trn->get_z_axis();
//     }
//     if (values_changed) {
//         values_updated();
//     }
// }
//
// void gearoenix::render::camera::ArcController::values_updated()
//{
// #ifdef GX_DEBUG_MODE
//     const auto up_len = up.square_length() - 1;
//     if (GX_IS_NOT_ZERO(up_len))
//         GX_UNEXPECTED
// #endif
//     vertical_angle = math::Numeric::clamp(vertical_angle, max_vertical_angle, min_vertical_angle);
//     distance = math::Numeric::clamp(distance, max_distance, min_distance);
//     auto ppd = up.normalized_perpendicular(direction);
//     {
//         const auto len = ppd.length();
//         if (GX_IS_ZERO(len)) {
//             ppd = up.normalized_perpendicular();
//         } else {
//             ppd /= len;
//         }
//     }
//     const auto cos = std::cos(vertical_angle);
//     auto sin = std::sqrt(1.0 - cos * cos);
//     if (vertical_angle < 0) {
//         sin = -sin;
//     }
//     direction = (up * (sin * distance)) + (ppd * (cos * distance));
//     trn->look_at(target + direction, target, up);
// }
//
// gearoenix::render::camera::ArcController::ArcController(std::shared_ptr<Camera> c)
//     : up(0.0, 0.0, 1.0)
//     , direction(1.0, 0.0, 0.0)
//     , controlled_camera(std::move(c))
//     , trn(controlled_camera->get_transformation())
//     , render_engine(controlled_camera->get_render_engine())
//     , function_id(render_engine->get_update_manager()->add({ controlled_camera->get_id() }, 0.0, [this] { update(); }))
//{
//     auto* const event_engine = render_engine->get_platform_application()->get_event_engine();
//     event_engine->add_listener(core::event::Id::GestureDrag2D, 0.0f, this);
//     event_engine->add_listener(core::event::Id::GestureScale, 0.0f, this);
//     event_engine->add_listener(core::event::Id::ButtonKeyboard, 0.0f, this);
//     values_updated();
// }
//
// gearoenix::render::camera::ArcController::~ArcController()
//{
//     render_engine->get_update_manager()->remove(function_id);
//     auto* const event_engine = render_engine->get_platform_application()->get_event_engine();
//     event_engine->remove_listener(core::event::Id::GestureDrag2D, 0.0f, this);
//     event_engine->remove_listener(core::event::Id::GestureScale, 0.0f, this);
//     event_engine->remove_listener(core::event::Id::ButtonKeyboard, 0.0f, this);
// }
//
// void gearoenix::render::camera::ArcController::set_vertical_angle(const double a)
//{
//     vertical_angle = a;
//     values_updated();
// }
//
// void gearoenix::render::camera::ArcController::set_min_vertical_angle(const double a)
//{
//     min_vertical_angle = a;
//     values_updated();
// }
//
// void gearoenix::render::camera::ArcController::set_max_vertical_angle(const double a)
//{
//     max_vertical_angle = a;
//     values_updated();
// }
//
// void gearoenix::render::camera::ArcController::set_distance(const double d)
//{
//     distance = d;
//     values_updated();
// }
//
// void gearoenix::render::camera::ArcController::set_min_distance(const double d)
//{
//     min_distance = d;
//     values_updated();
// }
//
// void gearoenix::render::camera::ArcController::set_max_distance(const double d)
//{
//     max_distance = d;
//     values_updated();
// }
//
// void gearoenix::render::camera::ArcController::set_target(const math::Vec3<double>& t)
//{
//     target = t;
//     values_updated();
// }
//
// bool gearoenix::render::camera::ArcController::on_event(const core::event::Data& d)
//{
//     switch (d.get_source()) {
//     case core::event::Id::GestureDrag2D: {
//         const auto& data = std::get<gearoenix::core::event::gesture::Drag2D>(d.get_data());
//         rotate_x += data.get_point().get_delta_previous_position().y;
//         rotate_z += data.get_point().get_delta_previous_position().x;
//         break;
//     }
//     case core::event::Id::GestureScale: {
//         auto new_distance = -std::get<gearoenix::core::event::gesture::Scale>(d.get_data()).get_delta_previous_scale();
//         new_distance *= touch_scale_speed;
//         new_distance += distance;
//         new_distance = math::Numeric::clamp(new_distance, max_distance, min_distance);
//         trn->local_z_translate(new_distance - distance);
//         distance = new_distance;
//         break;
//     }
//     case core::event::Id::ButtonKeyboard: {
//         const auto& data = std::get<core::event::button::KeyboardData>(d.get_data());
//         const bool pressed = data.get_action() == core::event::button::KeyboardActionId::Press;
//         switch (data.get_key()) {
//         case core::event::button::KeyboardKeyId::W: {
//             move_forward = pressed;
//             break;
//         }
//         case core::event::button::KeyboardKeyId::S: {
//             move_backward = pressed;
//             break;
//         }
//         case core::event::button::KeyboardKeyId::Up: {
//             move_forward = pressed;
//             break;
//         }
//         case core::event::button::KeyboardKeyId::Down: {
//             move_backward = pressed;
//             break;
//         }
//         case core::event::button::KeyboardKeyId::Left: {
//             rotate_left = pressed;
//             break;
//         }
//         case core::event::button::KeyboardKeyId::Right: {
//             rotate_right = pressed;
//             break;
//         }
//         default:
//             break;
//         }
//         break;
//     }
//     default:
//         GX_UNEXPECTED
//     }
//     return false;
// }
