#include "gx-rnd-wdg-widget.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../core/event/gx-cr-ev-engine.hpp"
#include "../../physics/collider/gx-phs-cld-aabb.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../camera/gx-rnd-cmr-camera.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../model/gx-rnd-mdl-model.hpp"
#include "gx-rnd-wdg-layout.hpp"
#include <array>

constexpr std::array default_event_ids {
    gearoenix::core::event::Id::GestureClick,
    gearoenix::core::event::Id::GestureDrag2D,
    gearoenix::core::event::Id::ButtonMouse,
    gearoenix::core::event::Id::MovementMouse,
    gearoenix::core::event::Id::PlatformWindowSizeChange,
    gearoenix::core::event::Id::Touch,
};

std::optional<gearoenix::math::Vec3<double>> gearoenix::render::widget::Widget::get_hit_point(const math::Vec2<double>& normalised_point) const
{
    const auto* const world = core::ecs::World::get();
    if (0 == model_entity_id) {
        return std::nullopt;
    }
    const auto* const collider = world->get_component<physics::collider::Aabb3>(model_entity_id);
    if (nullptr == collider) {
        return std::nullopt;
    }
    if (core::ecs::invalid_entity_id == camera_entity_id) {
        return std::nullopt;
    }
    const auto [camera_transform, camera] = world->get_components<physics::Transformation, camera::Camera>(camera_entity_id);
    if (nullptr == camera_transform || nullptr == camera) {
        return std::nullopt;
    }
    const auto ray = camera->generate_ray(normalised_point);
    if (auto dis = collider->get_updated_box().hit(ray, std::numeric_limits<double>::max()); dis.has_value()) {
        return ray.get_point(dis.value());
    }
    return std::nullopt;
}

void gearoenix::render::widget::Widget::handle_click_gesture(const core::event::Data& event_data)
{
    const auto& click = std::get<core::event::gesture::Click>(event_data.get_data());
    const auto hit = get_hit_point(click.get_point().get_current_position());
    if (!hit.has_value()) {
        return;
    }
    on_click(hit.value());
}

void gearoenix::render::widget::Widget::handle_button_mouse(const core::event::Data& event_data)
{
    const auto& mouse = std::get<core::event::button::Mouse>(event_data.get_data());
    const auto hit = get_hit_point(mouse.get_position_normalised());
    if (!hit.has_value()) {
        handle_cancel();
        return;
    }
    if (mouse.get_key() == platform::key::Id::Left) {
        if (mouse.get_action() == platform::key::Action::Press) {
            handle_press(*hit);
        } else if (mouse.get_action() == platform::key::Action::Release) {
            handle_release(*hit);
        }
    }
}

void gearoenix::render::widget::Widget::handle_movement_mouse(const core::event::Data& event_data)
{
    const auto& move = std::get<core::event::movement::Mouse>(event_data.get_data());
    const auto hit = get_hit_point(move.get_current_normalised_position());
    if (!hit.has_value()) {
        handle_cancel();
    }
}

void gearoenix::render::widget::Widget::handle_touch(const core::event::Data& event_data)
{
    if (e.get_platform_application().get_base().get_touch_states().size() > 1) {
        handle_cancel();
        return;
    }
    const auto& touch = std::get<core::event::touch::Data>(event_data.get_data());
    const auto hit = get_hit_point(touch.get_point().get_current_position());
    if (!hit.has_value()) {
        handle_cancel();
        return;
    }
    const auto action = touch.get_action();
    if (core::event::touch::Action::Down == action) {
        handle_press(*hit);
    } else if (core::event::touch::Action::Up == action) {
        handle_release(*hit);
    }
}

void gearoenix::render::widget::Widget::handle_cancel()
{
    if (is_pressed) {
        is_pressed = false;
        on_cancel();
    }
}

void gearoenix::render::widget::Widget::handle_press(const math::Vec3<double>& hit_point)
{
    is_pressed = true;
    on_press(hit_point);
}

void gearoenix::render::widget::Widget::handle_release(const math::Vec3<double>& hit_point)
{
    if (is_pressed) {
        is_pressed = false;
        on_release(hit_point);
    }
}

gearoenix::render::widget::Widget::Widget(
    std::string&& n,
    const Type widget_type,
    engine::Engine& e)
    : e(e)
    , name(std::move(n))
    , widget_type(widget_type)
    , on_press([](const math::Vec3<double>&) -> void { })
    , on_release([](const math::Vec3<double>&) -> void { })
    , on_click([](const math::Vec3<double>&) -> void { })
    , on_cancel([]() -> void { })
{
}

gearoenix::render::widget::Widget::~Widget()
{
    auto& ee = *e.get_platform_application().get_base().get_event_engine();
    for (const auto event_id : default_event_ids) {
        ee.remove_listener(event_id, this);
    }
}

void gearoenix::render::widget::Widget::set_on_press(const std::function<void(const math::Vec3<double>&)>& fun)
{
    on_press = fun;
}

void gearoenix::render::widget::Widget::set_on_release(const std::function<void(const math::Vec3<double>&)>& fun)
{
    on_release = fun;
}

void gearoenix::render::widget::Widget::set_on_cancel(const std::function<void()>& fun)
{
    on_cancel = fun;
}

void gearoenix::render::widget::Widget::set_on_click(const std::function<void(const math::Vec3<double>&)>& fun)
{
    on_click = fun;
}

void gearoenix::render::widget::Widget::set_sensitivity(const bool b)
{
    sensitivity = b;
    for (const auto& child : children) {
        child.second->set_sensitivity(b);
    }
}

void gearoenix::render::widget::Widget::set_model_entity_id(const core::ecs::entity_id_t id)
{
    model_entity_id = id;
}

void gearoenix::render::widget::Widget::set_camera_entity_id(const core::ecs::entity_id_t id)
{
    camera_entity_id = id;
}

void gearoenix::render::widget::Widget::register_for_events()
{
    auto& ee = *e.get_platform_application().get_base().get_event_engine();
    for (const auto event_id : default_event_ids) {
        ee.add_listener(event_id, this);
    }
}

void gearoenix::render::widget::Widget::set_layout(std::shared_ptr<Layout> l)
{
    layout = std::move(l);
}

void gearoenix::render::widget::Widget::show()
{
    auto* const mdl = core::ecs::World::get()->get_component<model::Model>(model_entity_id);
    GX_ASSERT_D(nullptr != mdl);
    mdl->set_enabled(true);
    for (auto& child : children) {
        child.second->show();
    }
    set_sensitivity(true);
}

void gearoenix::render::widget::Widget::hide()
{
    auto* const mdl = core::ecs::World::get()->get_component<model::Model>(model_entity_id);
    GX_ASSERT_D(nullptr != mdl);
    mdl->set_enabled(false);
    for (auto& child : children) {
        child.second->hide();
    }
    set_sensitivity(false);
}

void gearoenix::render::widget::Widget::add_child(std::shared_ptr<Widget>&& child, const double priority)
{
    children.emplace(std::make_pair(priority, child->name), std::move(child));
}

void gearoenix::render::widget::Widget::add_child(std::shared_ptr<Widget>&& child)
{
    add_child(std::move(child), 0.0);
    transform->add_child(child->transform.get());
}

gearoenix::core::event::Listener::Response gearoenix::render::widget::Widget::on_event(
    const core::event::Data& event_data)
{
    switch (event_data.get_source()) {
    case core::event::Id::GestureClick: {
        if (!sensitivity)
            return Response::Continue;
        handle_click_gesture(event_data);
        break;
    }
    case core::event::Id::GestureDrag2D: {
        if (!sensitivity)
            return Response::Continue;
        break;
    }
    case core::event::Id::MovementMouse: {
        if (!sensitivity)
            return Response::Continue;
        handle_movement_mouse(event_data);
        break;
    }
    case core::event::Id::ButtonMouse: {
        if (!sensitivity)
            return Response::Continue;
        handle_button_mouse(event_data);
        break;
    }
    case core::event::Id::PlatformWindowSizeChange: {
        if (nullptr != layout)
            (void)layout->on_event(event_data);
        break;
    }
    case core::event::Id::Touch: {
        if (!sensitivity)
            return Response::Continue;
        handle_touch(event_data);
    }
    default:
        if (!sensitivity)
            return Response::Continue;
        break;
    }
    for (const auto& child : children) {
        const auto res = child.second->on_event(event_data);
        if (res == Response::Continue) {
            continue;
        }
        return res;
    }
    return Response::Continue;
}
