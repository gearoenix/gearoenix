#include "gx-phs-cns-jet-controller.hpp"
#include "../../core/allocator/gx-cr-alc-shared-array.hpp"
#include "../../core/event/gx-cr-ev-engine.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../../render/engine/gx-rnd-eng-engine.hpp"
#include "../../render/gizmo/gx-rnd-gzm-manager.hpp"
#include "../gx-phs-transformation.hpp"
#include <imgui/imgui.h>

namespace {
const auto allocator = gearoenix::core::allocator::SharedArray<gearoenix::physics::constraint::JetController, 64>::construct();
}

gearoenix::core::event::Listener::Response gearoenix::physics::constraint::JetController::on_event(const core::event::Data& d)
{
    switch (d.get_source()) {
    case core::event::Id::ButtonMouse: {
        const auto& data = std::get<core::event::button::Mouse>(d.get_data());
        if (data.get_key() == platform::key::Id::Left) {
            rotate = data.get_action() == platform::key::Action::Press;
        }
        break;
    }
    case core::event::Id::MovementMouse: {
        const auto& data = std::get<core::event::movement::Mouse>(d.get_data());
        if (rotate) {
            const auto& delta = data.get_delta_normalised_position();
            rotate_x += delta.y;
            rotate_z += delta.x;
        }
        break;
    }
    case core::event::Id::GestureDrag2D: {
        const auto& data = std::get<core::event::gesture::Drag2D>(d.get_data());
        rotate_x += data.get_point().get_delta_previous_position().y;
        rotate_z += data.get_point().get_delta_previous_position().x;
        break;
    }
    case core::event::Id::GestureScale: {
        move_forward_accumulated -= std::get<core::event::gesture::Scale>(d.get_data()).get_delta_previous_scale();
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
        GX_UNEXPECTED;
    }
    return Response::Continue;
}

const gearoenix::core::ecs::Component::HierarchyTypes& gearoenix::physics::constraint::JetController::get_hierarchy_types() const
{
    static const auto types = generate_hierarchy_types<Constraint>(this);
    return types;
}

gearoenix::physics::constraint::JetController::JetController(
    render::engine::Engine& e,
    std::shared_ptr<Transformation>&& trn,
    std::string&& name,
    const core::ecs::entity_id_t entity_id)
    : Constraint(create_this_type_index(this), std::move(name), entity_id)
    , transformation(std::move(trn))
    , e(e)
{
    auto* const event_engine = e.get_platform_application().get_base().get_event_engine();
    event_engine->add_listener(core::event::Id::GestureDrag2D, this);
    event_engine->add_listener(core::event::Id::GestureScale, this);
    event_engine->add_listener(core::event::Id::ButtonKeyboard, this);
    event_engine->add_listener(core::event::Id::ButtonMouse, this);
    event_engine->add_listener(core::event::Id::MovementMouse, this);
}

std::shared_ptr<gearoenix::physics::constraint::JetController> gearoenix::physics::constraint::JetController::construct(
    render::engine::Engine& e, std::shared_ptr<Transformation>&& trn, std::string&& name, const core::ecs::entity_id_t entity_id)
{
    auto self = allocator->make_shared(e, std::move(trn), std::move(name), entity_id);
    self->set_component_self(self);
    return self;
}

gearoenix::physics::constraint::JetController::~JetController()
{
    auto* const event_engine = e.get_platform_application().get_base().get_event_engine();
    event_engine->remove_listener(core::event::Id::GestureDrag2D, 0.0f, this);
    event_engine->remove_listener(core::event::Id::GestureScale, 0.0f, this);
    event_engine->remove_listener(core::event::Id::ButtonKeyboard, 0.0f, this);
    event_engine->remove_listener(core::event::Id::ButtonMouse, 0.0f, this);
    event_engine->remove_listener(core::event::Id::MovementMouse, 0.0f, this);
}

void gearoenix::physics::constraint::JetController::update()
{

    if (e.get_gizmo_manager()->is_processing_inputs()) {
        clear_transforms();
        return;
    }

    if (const auto& io = ImGui::GetIO();
        (io.WantCaptureMouse || io.WantCaptureKeyboard) && (ImGui::IsAnyItemActive() || ImGui::IsAnyItemFocused())) {
        clear_transforms();
        return;
    }

    const auto delta_time = e.get_delta_time();
    if (0.0 != rotate_x) {
        transformation->local_x_rotate(rotate_x);
    }
    if (0.0 != rotate_z) {
        transformation->local_rotate(rotate_z, math::Z3D<double>);
    }
    if (0.0 != move_forward_accumulated) {
        transformation->local_z_translate(move_forward_accumulated);
    }
    const auto delta_movement = movement_speed * delta_time;
    const auto delta_rotation = rotation_speed * delta_time;
    if (move_forward) {
        transformation->local_z_translate(-delta_movement);
    }
    if (move_backward) {
        transformation->local_z_translate(delta_movement);
    }
    if (move_rightward) {
        transformation->local_x_translate(delta_movement);
    }
    if (move_leftward) {
        transformation->local_x_translate(-delta_movement);
    }
    if (rotate_right) {
        transformation->local_rotate(-delta_rotation, math::Z3D<double>);
    }
    if (rotate_left) {
        transformation->local_rotate(delta_rotation, math::Z3D<double>);
    }
    clear_transforms();
}

void gearoenix::physics::constraint::JetController::clear_transforms()
{
    rotate_x = 0.0;
    rotate_z = 0.0;
    move_forward_accumulated = 0.0;
}
