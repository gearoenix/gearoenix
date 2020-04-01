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
    , function_id(render_engine->get_update_functions_manager()->add(std::bind(&JetController::update, this)))
{
    auto* const event_engine = render_engine->get_system_application()->get_event_engine();
    event_engine->add_listener(core::event::Id::ButtonMouse, 0.0f, this);
    event_engine->add_listener(core::event::Id::MovementMouse, 0.0f, this);
    event_engine->add_listener(core::event::Id::ButtonKeyboard, 0.0f, this);
}

gearoenix::render::camera::JetController::~JetController() noexcept
{
    render_engine->get_update_functions_manager()->remove(function_id);
}

bool gearoenix::render::camera::JetController::on_event(const core::event::Data& d) noexcept
{
    switch (d.source) {
    case core::event::Id::ButtonMouse: {
        const auto& data = std::get<core::event::button::MouseData>(d.data);
        if (data.key == core::event::button::MouseKeyId::Left)
            camera_rotation_enabled = data.action == core::event::button::MouseActionId::Press;
        break;
    }
    case core::event::Id::MovementMouse: {
        if (camera_rotation_enabled) {
            const auto& data = std::get<gearoenix::core::event::movement::Base>(d.data);
            rotate_x += data.delta_position[1];
            rotate_z += data.delta_position[0];
        }
        break;
    }
    case core::event::Id::ButtonKeyboard: {
        const auto& data = std::get<core::event::button::KeyboardData>(d.data);
        const bool pressed = data.action == core::event::button::KeyboardActionId::Press;
        switch (data.key) {
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
        GXUNEXPECTED
    }
    return false;
}
