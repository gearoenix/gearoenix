#include "rnd-cmr-jet-controller.hpp"
#include "../../core/cr-update-functions-manager.hpp"
#include "../../core/event/cr-ev-engine.hpp"
#include "../../system/sys-app.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "rnd-cmr-camera.hpp"
#include "rnd-cmr-transformation.hpp"

void gearoenix::render::camera::JetController::update() noexcept
{
    if (rotate_x != 0.0) {
        trn->local_x_rotate(rotate_x);
        rotate_x = 0.0;
    }
    if (rotate_z != 0.0) {
        trn->local_rotate(rotate_z, math::Vec3(0.0, 0.0, 1.0));
        rotate_z = 0.0;
    }
    const auto delta_movement = movement_speed * render_engine->get_delta_time();
    const auto delta_rotation = rotation_speed * render_engine->get_delta_time();
    if (move_forward) {
        trn->local_z_translate(-delta_movement);
    }
    if (move_backward) {
        trn->local_z_translate(delta_movement);
    }
    if (move_rightward) {
        trn->local_x_translate(delta_movement);
    }
    if (move_leftward) {
        trn->local_x_translate(-delta_movement);
    }
    if (rotate_right) {
        trn->local_rotate(-delta_rotation, math::Vec3(0.0, 0.0, 1.0));
    }
    if (rotate_left) {
        trn->local_rotate(delta_rotation, math::Vec3(0.0, 0.0, 1.0));
    }
}

gearoenix::render::camera::JetController::JetController(std::shared_ptr<Camera> c) noexcept
    : trn(c->get_transformation())
    , render_engine(c->get_render_engine())
    , cam(std::move(c))
    , function_id(render_engine->get_update_functions_manager()->add([this] { update(); }))
{
    auto* const event_engine = render_engine->get_system_application()->get_event_engine();
    event_engine->add_listener(core::event::Id::GestureDrag2D, 0.0f, this);
    event_engine->add_listener(core::event::Id::GestureScale, 0.0f, this);
    event_engine->add_listener(core::event::Id::ButtonKeyboard, 0.0f, this);
}

gearoenix::render::camera::JetController::~JetController() noexcept
{
    render_engine->get_update_functions_manager()->remove(function_id);
    auto* const event_engine = render_engine->get_system_application()->get_event_engine();
    event_engine->remove_listener(core::event::Id::GestureDrag2D, 0.0f, this);
    event_engine->remove_listener(core::event::Id::GestureScale, 0.0f, this);
    event_engine->remove_listener(core::event::Id::ButtonKeyboard, 0.0f, this);
}

bool gearoenix::render::camera::JetController::on_event(const core::event::Data& d) noexcept
{
    switch (d.get_source()) {
    case core::event::Id::GestureDrag2D: {
        const auto& data = std::get<gearoenix::core::event::gesture::Drag2D>(d.get_data());
        rotate_x += data.get_point().get_delta_previous_position().y;
        rotate_z += data.get_point().get_delta_previous_position().x;
        break;
    }
    case core::event::Id::GestureScale: {
        trn->local_z_translate(-std::get<gearoenix::core::event::gesture::Scale>(d.get_data()).get_delta_previous_scale());
        break;
    }
    case core::event::Id::ButtonKeyboard: {
        const auto& data = std::get<core::event::button::KeyboardData>(d.get_data());
        const bool pressed = data.get_action() == core::event::button::KeyboardActionId::Press;
        switch (data.get_key()) {
        case core::event::button::KeyboardKeyId::W: {
            move_forward = pressed;
            break;
        }
        case core::event::button::KeyboardKeyId::S: {
            move_backward = pressed;
            break;
        }
        case core::event::button::KeyboardKeyId::D: {
            move_rightward = pressed;
            break;
        }
        case core::event::button::KeyboardKeyId::A: {
            move_leftward = pressed;
            break;
        }
        case core::event::button::KeyboardKeyId::Up: {
            move_forward = pressed;
            break;
        }
        case core::event::button::KeyboardKeyId::Down: {
            move_backward = pressed;
            break;
        }
        case core::event::button::KeyboardKeyId::Left: {
            rotate_left = pressed;
            break;
        }
        case core::event::button::KeyboardKeyId::Right: {
            rotate_right = pressed;
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        GX_UNEXPECTED
    }
    return false;
}
