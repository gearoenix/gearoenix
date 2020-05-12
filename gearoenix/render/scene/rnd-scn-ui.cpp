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

void gearoenix::render::scene::Ui::pointer_down(const double x, const double y) noexcept
{
    selected_widget = nullptr;
    find_hit_widgets(
        x, y,
        [this](widget::Widget* const wdg, const math::Vec3<double>& p) noexcept {
            wdg->selected(p);
            selected_widget = wdg;
            if (selected_widget->get_widget_type() == widget::Type::Edit && selected_edit != selected_widget) {
                auto* const select = dynamic_cast<widget::Edit*>(selected_widget);
                if (selected_edit != nullptr) {
                    selected_edit->active(false);
                }
                select->active();
                selected_edit = select;
            }
        },
        [this](widget::Widget* const wdg, const math::Vec3<double>& p,
            const std::vector<model::Model*>& children) noexcept {
            wdg->selected_on(p, children);
            if (selected_widget == nullptr)
                selected_widget = wdg;
        },
        []() noexcept {});
}

void gearoenix::render::scene::Ui::pointer_up() noexcept
{
    if (selected_widget != nullptr) {
        selected_widget->select_released();
        selected_widget = nullptr;
    }
}

void gearoenix::render::scene::Ui::pointer_move(const double x, const double y) noexcept
{
    if (selected_widget == nullptr)
        return;
    bool widget_found = false;
    find_hit_widgets(
        x, y,
        [&](widget::Widget* const wdg, const math::Vec3<double>& p) noexcept {
            if (selected_widget == wdg) {
                widget_found = true;
                selected_widget->dragged(p);
            } else if (nullptr != selected_widget) {
                selected_widget->select_cancelled();
                selected_widget = nullptr;
            }
        },
        [&](widget::Widget* const wdg, const math::Vec3<double>& p, const std::vector<model::Model*>& children) noexcept {
            if (widget_found)
                return;
            if (selected_widget == wdg) {
                widget_found = true;
                selected_widget->dragged_on(p, children);
            } else if (nullptr != selected_widget) {
                selected_widget->select_cancelled();
                selected_widget = nullptr;
            }
        },
        [&]() noexcept {
            if (selected_widget != nullptr) {
                selected_widget->select_cancelled();
                selected_widget = nullptr;
            }
        });
}

void gearoenix::render::scene::Ui::init() noexcept
{
    layer = 0.1f;
    auto* const sys_app = e->get_system_application();
    auto cam = sys_app->get_asset_manager()->get_camera_manager()->create<camera::Orthographic>();
    add_camera(cam);
    cam->get_transformation()->set_location(math::Vec3(0.0, 0.0, 50.0));
    uniform.ambient_light = math::Vec4(1.0f, 1.0f, 1.0f, 1.0f);
    auto* const event_engine = sys_app->get_event_engine();
    event_engine->add_listener(core::event::Id::ButtonMouse, 0.0, this);
    event_engine->add_listener(core::event::Id::MovementMouse, 0.0, this);
    event_engine->add_listener(core::event::Id::Touch, 0.0, this);
}

gearoenix::render::scene::Ui::Ui(const core::Id my_id, system::stream::Stream* f, engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Scene(my_id, Type::Ui, f, e, c)
{
    init();
}

gearoenix::render::scene::Ui::Ui(const core::Id my_id, engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Scene(my_id, Type::Ui, e, c)
{
    init();
}

bool gearoenix::render::scene::Ui::on_event(const core::event::Data& d) noexcept
{
    switch (d.get_source()) {
    case core::event::Id::ButtonMouse: {
        const auto& data = std::get<core::event::button::MouseData>(d.get_data());
        if (data.get_key() == core::event::button::MouseKeyId::Left) {
            if (data.get_action() == core::event::button::MouseActionId::Press) {
                const auto& p = data.get_position();
                pointer_down(p.x, p.y);
            } else if (data.get_action() == core::event::button::MouseActionId::Release) {
                pointer_up();
            }
        }
        break;
    }
    case core::event::Id::MovementMouse: {
        const auto& p = std::get<core::event::movement::Base2D>(d.get_data()).get_point().get_current_position();
        pointer_move(p.x, p.y);
        break;
    }
    case core::event::Id::Touch: {
        const auto& data = std::get<core::event::touch::Data>(d.get_data());
        switch (data.get_action()) {
        case core::event::touch::Action::Down: {
            if (e->get_system_application()->get_event_engine()->get_touch_states().size() == 1) {
                const auto& p = data.get_state().get_point().get_current_position();
                pointer_down(p.x, p.y);
            }
            break;
        }
        case core::event::touch::Action::Up: {
            pointer_up();
            break;
        }
        case core::event::touch::Action::Move:
            if (e->get_system_application()->get_event_engine()->get_touch_states().size() == 1) {
                const auto& p = data.get_state().get_point().get_current_position();
                pointer_move(p.x, p.y);
            }
            break;
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
        auto* const w = dynamic_cast<widget::Widget*>(m.get());
        if (w->get_widget_type() == widget::Type::Edit) {
            auto* const edt = dynamic_cast<widget::Edit*>(w);
            if (selected_edit == nullptr) {
                selected_edit = edt;
            } else {
                edt->active(false);
            }
        }
    }
}
