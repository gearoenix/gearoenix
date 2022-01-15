#include "gx-rnd-cmr-jet-controller.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../core/event/gx-cr-ev-engine.hpp"
#include "../../core/sync/gx-cr-sync-update-manager.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "gx-rnd-cmr-camera.hpp"

gearoenix::core::event::Listener::Response gearoenix::render::camera::JetController::on_event(const core::event::Data& d) noexcept
{
    switch (d.get_source()) {
    case core::event::Id::ButtonMouse: {
        const auto& data = std::get<gearoenix::core::event::button::Mouse>(d.get_data());
        if (data.get_key() == platform::key::Id::Middle)
            rotate = data.get_action() == platform::key::Action::Press;
        break;
    }
    case core::event::Id::MovementMouse: {
        const auto& data = std::get<gearoenix::core::event::movement::Mouse>(d.get_data());
        if (rotate) {
            const auto& delta = data.get_delta_normalised_position();
            rotate_x += delta.y;
            rotate_z += delta.x;
        }
        break;
    }
    case core::event::Id::GestureDrag2D: {
        const auto& data = std::get<gearoenix::core::event::gesture::Drag2D>(d.get_data());
        rotate_x += data.get_point().get_delta_previous_position().y;
        rotate_z += data.get_point().get_delta_previous_position().x;
        break;
    }
    case core::event::Id::GestureScale: {
        move_forward_accumulated -= std::get<gearoenix::core::event::gesture::Scale>(d.get_data()).get_delta_previous_scale();
        break;
    }
    case core::event::Id::ButtonKeyboard: {
        const auto& data = std::get<core::event::button::Keyboard>(d.get_data());
        const bool pressed = data.get_action() == platform::key::Action::Press;
        switch (data.get_key()) {
        case platform::key::Id::W: {
            move_forward = pressed;
            break;
        }
        case platform::key::Id::S: {
            move_backward = pressed;
            break;
        }
        case platform::key::Id::D: {
            move_rightward = pressed;
            break;
        }
        case platform::key::Id::A: {
            move_leftward = pressed;
            break;
        }
        case platform::key::Id::Up: {
            move_forward = pressed;
            break;
        }
        case platform::key::Id::Down: {
            move_backward = pressed;
            break;
        }
        case platform::key::Id::Left: {
            rotate_left = pressed;
            break;
        }
        case platform::key::Id::Right: {
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
    return Response::Continue;
}

gearoenix::render::camera::JetController::JetController(engine::Engine& e, const core::ecs::Entity::id_t camera_entity_id) noexcept
    : camera_entity_id(camera_entity_id)
    , e(e)
{
    auto* const event_engine = e.get_platform_application().get_base().get_event_engine();
    event_engine->add_listener(core::event::Id::GestureDrag2D, this);
    event_engine->add_listener(core::event::Id::GestureScale, this);
    event_engine->add_listener(core::event::Id::ButtonKeyboard, this);
    event_engine->add_listener(core::event::Id::ButtonMouse, this);
    event_engine->add_listener(core::event::Id::MovementMouse, this);
}

gearoenix::render::camera::JetController::~JetController() noexcept
{
    auto* const event_engine = e.get_platform_application().get_base().get_event_engine();
    event_engine->remove_listener(core::event::Id::GestureDrag2D, 0.0f, this);
    event_engine->remove_listener(core::event::Id::GestureScale, 0.0f, this);
    event_engine->remove_listener(core::event::Id::ButtonKeyboard, 0.0f, this);
    event_engine->remove_listener(core::event::Id::ButtonMouse, 0.0f, this);
    event_engine->remove_listener(core::event::Id::MovementMouse, 0.0f, this);
}

void gearoenix::render::camera::JetController::update() noexcept
{
    auto* const trn = e.get_world()->get_component<physics::Transformation>(camera_entity_id);
    const auto delta_time = e.get_delta_time();
    if (nullptr == trn) {
        GX_LOG_D("Jet camera controller is not able to find the transformation component of the camera: " << camera_entity_id)
        return;
    }
    if (0.0 != rotate_x) {
        trn->local_x_rotate(rotate_x);
        rotate_x = 0.0;
    }
    if (0.0 != rotate_z) {
        trn->local_rotate(rotate_z, math::Z3D<double>);
        rotate_z = 0.0;
    }
    if (0.0 != move_forward_accumulated) {
        trn->local_z_translate(move_forward_accumulated);
        move_forward_accumulated = 0.0;
    }
    const auto delta_movement = movement_speed * delta_time;
    const auto delta_rotation = rotation_speed * delta_time;
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
        trn->local_rotate(-delta_rotation, math::Z3D<double>);
    }
    if (rotate_left) {
        trn->local_rotate(delta_rotation, math::Z3D<double>);
    }
}