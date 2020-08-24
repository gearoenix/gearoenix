#include "rnd-scn-ui.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/event/cr-ev-engine.hpp"
#include "../../physics/collider/phs-cld-collider.hpp"
#include "../../physics/phs-engine.hpp"
#include "../../system/sys-app.hpp"
#include "../camera/rnd-cmr-manager.hpp"
#include "../camera/rnd-cmr-orthographic.hpp"
#include "../camera/rnd-cmr-transformation.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../model/rnd-mdl-model.hpp"
#include "../widget/rnd-wdg-edit.hpp"

void gearoenix::render::scene::Ui::find_hit_widgets(
    const double x, const double y,
    const std::function<void(widget::Widget* const, const math::Vec3<double>&)>& f1,
    const std::function<void(widget::Widget* const, const math::Vec3<double>&, const std::vector<model::Model*>&)>& f2,
    const std::function<void()>& f3) const noexcept
{
    const auto ray = cameras.begin()->second->create_ray3(x, y);
    auto h = hit(ray, std::numeric_limits<double>::max());
    if (h.has_value()) {
        const double d_ray = h.value().first;
        const math::Vec3 point = ray.get_point(d_ray);
        auto* const cld = h.value().second;
        auto* const mdl = cld->get_parent();
        std::vector<model::Model*> children;
        if (mdl != nullptr) {
            if (mdl->get_model_type() == model::Type::Widget) {
                auto* const wdg = dynamic_cast<widget::Widget*>(mdl);
                f1(wdg, point);
            }
            children.push_back(mdl);
            for (auto* parent = mdl->get_parent(); parent != nullptr; parent = parent->get_parent()) {
                if (parent->get_model_type() == model::Type::Widget) {
                    auto* const wdg = dynamic_cast<widget::Widget*>(parent);
                    f2(wdg, point, children);
                }
                children.push_back(parent);
            }
        }
    } else {
        f3();
    }
}

void gearoenix::render::scene::Ui::pointer_down(const double x, const double y, const core::event::touch::FingerId finger_id) noexcept
{
    bool widget_not_found = true;
    const auto f_select = [&](widget::Widget* const wgd, const math::Vec3<double>& p) noexcept {
        const auto selected_widgets_search = selected_widgets.find(finger_id);
        const auto f = [&] {
            wgd->selected(p);
            selected_widgets.emplace(finger_id, wgd->get_widget_self());
        };
        std::shared_ptr<widget::Widget> pre_wdg;
        if (selected_widgets.end() != selected_widgets_search && (pre_wdg = selected_widgets_search->second.lock()) && pre_wdg != nullptr) {
            if (pre_wdg->get_id() != wgd->get_id()) {
                pre_wdg->select_cancelled();
                f();
            }
        } else {
            f();
        }
    };

    find_hit_widgets(
        x, y,
        [&](
            widget::Widget* const wgd,
            const math::Vec3<double>& p) noexcept {
            widget_not_found = false;
            f_select(wgd, p);
            if (wgd->get_widget_type() == widget::Type::Edit) {
                const auto e_select = [&] {
                    auto* const selected_edit = dynamic_cast<widget::Edit*>(wgd);
                    selected_edit->active();
                };
                if (const auto edit = active_edit.lock()) {
                    if (edit->get_id() != wgd->get_id()) {
                        edit->active(false);
                        e_select();
                    }
                } else {
                    e_select();
                }
            } else {
                e->get_system_application()->set_soft_keyboard_visibility(false);
            }
        },
        [&](
            widget::Widget* const wdg,
            const math::Vec3<double>& p,
            const std::vector<model::Model*>& children) noexcept {
            wdg->selected_on(p, children);
            if (widget_not_found) {
                f_select(wdg, p);
                widget_not_found = false;
            }
        },
        []() noexcept {});
    if (widget_not_found) {
        e->get_system_application()->set_soft_keyboard_visibility(false);
    }
}

void gearoenix::render::scene::Ui::pointer_up(const core::event::touch::FingerId finger_id) noexcept
{
    const auto search = selected_widgets.find(finger_id);
    if (search == selected_widgets.end())
        return;
    if (const auto wdg = search->second.lock()) {
        wdg->select_released();
    }
    selected_widgets.erase(search);
}

void gearoenix::render::scene::Ui::pointer_move(const double x, const double y, const core::event::touch::FingerId finger_id) noexcept
{
    const auto search = selected_widgets.find(finger_id);
    if (search == selected_widgets.end())
        return;
    auto selected_widget = search->second.lock();
    if (nullptr == selected_widget) {
        selected_widgets.erase(search);
        return;
    }
    bool widget_found = false;
    const auto cancel_func = [&] {
        if (nullptr != selected_widget) {
            selected_widget->select_cancelled();
            selected_widget = nullptr;
        }
        selected_widgets.erase(search);
    };
    find_hit_widgets(
        x, y,
        [&](widget::Widget* const wdg, const math::Vec3<double>& p) noexcept {
            if (selected_widget.get() == wdg) {
                widget_found = true;
                selected_widget->dragged(p);
            } else
                cancel_func();
        },
        [&](widget::Widget* const wdg, const math::Vec3<double>& p, const std::vector<model::Model*>& children) noexcept {
            if (widget_found)
                return;
            if (selected_widget.get() == wdg) {
                widget_found = true;
                selected_widget->dragged_on(p, children);
            } else
                cancel_func();
        },
        [&]() noexcept {
            cancel_func();
        });
}

void gearoenix::render::scene::Ui::init() noexcept
{
    layer = 0.1;
    auto* const sys_app = e->get_system_application();
    auto cam = sys_app->get_asset_manager()->get_camera_manager()->create<camera::Orthographic>(
        "ui-scene-" + name + "-ortho");
    add_camera(cam);
    cam->get_transformation()->set_location(math::Vec3(0.0, 0.0, 50.0));
    uniform.ambient_light = math::Vec4(1.0f, 1.0f, 1.0f, 1.0f);
}

gearoenix::render::scene::Ui::Ui(
    const core::Id my_id,
    std::string name,
    system::stream::Stream* const f,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Scene(my_id, std::move(name), Type::Ui, f, e, c)
{
    init();
}

gearoenix::render::scene::Ui::Ui(
    const core::Id my_id,
    std::string name,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Scene(my_id, std::move(name), Type::Ui, e, c)
{
    init();
}

gearoenix::render::scene::Ui::~Ui() noexcept
{
    Ui::set_enabled(false);
}

bool gearoenix::render::scene::Ui::on_event(const core::event::Data& d) noexcept
{
    GX_GUARD_LOCK(events)
    switch (d.get_source()) {
    case core::event::Id::ButtonMouse: {
        const auto& data = std::get<core::event::button::MouseData>(d.get_data());
        if (data.get_key() == core::event::button::MouseKeyId::Left) {
            if (data.get_action() == core::event::button::MouseActionId::Press) {
                const auto& p = data.get_position();
                pointer_down(p.x, p.y, std::numeric_limits<core::event::touch::FingerId>::max());
            } else if (data.get_action() == core::event::button::MouseActionId::Release) {
                pointer_up(std::numeric_limits<core::event::touch::FingerId>::max());
            }
        }
        break;
    }
    case core::event::Id::MovementMouse: {
        const auto& p = std::get<core::event::movement::Base2D>(d.get_data()).get_point().get_current_position();
        pointer_move(p.x, p.y, std::numeric_limits<core::event::touch::FingerId>::max());
        break;
    }
    case core::event::Id::Touch: {
        const auto& data = std::get<core::event::touch::Data>(d.get_data());
        switch (data.get_action()) {
        case core::event::touch::Action::Down: {
            const auto& s = data.get_state();
            const auto& p = s.get_point().get_current_position();
            pointer_down(p.x, p.y, s.get_finger_id());
            break;
        }
        case core::event::touch::Action::Up: {
            pointer_up(data.get_state().get_finger_id());
            break;
        }
        case core::event::touch::Action::Move: {
            const auto& s = data.get_state();
            const auto& p = s.get_point().get_current_position();
            pointer_move(p.x, p.y, s.get_finger_id());
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        GXLOGF("Unexpected event happened.")
    }
    return false;
}

void gearoenix::render::scene::Ui::add_model(const std::shared_ptr<model::Model>& m) noexcept
{
    Scene::scene_add_model(m);
    if (m->get_model_type() == model::Type::Widget) {
        // It doesn't matter because it's not gonna happen very much.
        auto* const edit = dynamic_cast<widget::Edit*>(m.get());
        if (nullptr != edit) {
            if (active_edit.expired()) {
                active_edit = edit->get_edit_self();
            } else {
                edit->active(false);
            }
        }
    }
}

void gearoenix::render::scene::Ui::set_enabled(const bool enb) noexcept
{
    if (enb == enability)
        return;
    Scene::set_enabled(enb);
    auto* const event_engine = e->get_system_application()->get_event_engine();
    if (enb) {
        event_engine->add_listener(core::event::Id::ButtonMouse, 0.0, this);
        event_engine->add_listener(core::event::Id::MovementMouse, 0.0, this);
        event_engine->add_listener(core::event::Id::Touch, 0.0, this);
    } else {
        event_engine->remove_listener(core::event::Id::ButtonMouse, 0.0, this);
        event_engine->remove_listener(core::event::Id::MovementMouse, 0.0, this);
        event_engine->remove_listener(core::event::Id::Touch, 0.0, this);
    }
}
