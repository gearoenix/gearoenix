#include "rnd-scn-ui.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/event/cr-ev-engine.hpp"
#include "../../core/event/cr-ev-event.hpp"
#include "../../physics/collider/phs-cld-collider.hpp"
#include "../../physics/phs-engine.hpp"
#include "../../system/sys-app.hpp"
#include "../camera/rnd-cmr-camera.hpp"
#include "../camera/rnd-cmr-manager.hpp"
#include "../camera/rnd-cmr-orthographic.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../model/rnd-mdl-model.hpp"
#include "../widget/rnd-wdg-edit.hpp"
#include "../widget/rnd-wdg-widget.hpp"
#include <limits>

void gearoenix::render::scene::Ui::init() noexcept
{
    auto* const sys_app = e->get_system_application();
    auto cam = sys_app->get_asset_manager()->get_camera_manager()->create<camera::Orthographic>();
    add_camera(cam);
    cam->get_transformation()->set_location(math::Vec3(0.0f, 0.0f, 50.0f));
    uniform.ambient_light = math::Vec4(1.0f, 1.0f, 1.0f, 1.0f);
    auto* const event_engine = sys_app->get_event_engine();
    event_engine->add_listner(core::event::Id::ButtonMouse, 0.0f, this);
    event_engine->add_listner(core::event::Id::MovementMouse, 0.0f, this);
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
    auto find_hited_widgets = [this](
                                  const core::Real x,
                                  const core::Real y,
                                  const std::function<void(widget::Widget* const, const math::Vec3&)>& f1,
                                  const std::function<void(widget::Widget* const, const math::Vec3&, const std::vector<model::Model*>&)>& f2,
                                  const std::function<void()>& f3) noexcept {
        const auto ray = cameras.begin()->second->create_ray3(x, y);
        auto h = hit(ray, std::numeric_limits<gearoenix::core::Real>::max());
        if (h.has_value()) {
            const core::Real d_ray = h.value().first;
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
    };

    switch (d.source) {
    case core::event::Id::ButtonMouse: {
        const auto& data = std::get<gearoenix::core::event::button::MouseData>(d.data);
        if (data.key == core::event::button::MouseKeyId::Left) {
            if (data.action == core::event::button::MouseActionId::Press) {
                selected_widget = nullptr;
                find_hited_widgets(
                    data.position[0], data.position[1],
                    [this](widget::Widget* const wdg, const math::Vec3& p) noexcept {
                        wdg->selected(p);
                        selected_widget = wdg;
                        if (selected_widget->get_widget_type() == widget::Type::Edit && selected_edit != selected_widget) {
                            auto* const seledt = dynamic_cast<widget::Edit*>(selected_widget);
                            if (selected_edit != nullptr) {
                                selected_edit->active(false);
                            }
                            seledt->active();
                            selected_edit = seledt;
                        }
                    },
                    [this](widget::Widget* const wdg, const math::Vec3& p,
                        const std::vector<model::Model*>& children) noexcept {
                        wdg->selected_on(p, children);
                        if (selected_widget == nullptr)
                            selected_widget = wdg;
                    },
                    []() noexcept {});
            } else if (data.action == core::event::button::MouseActionId::Release) {
                if (selected_widget != nullptr) {
                    selected_widget->select_released();
                    selected_widget = nullptr;
                }
            }
        }
        break;
    }
    case core::event::Id::MovementMouse: {
        const auto& data = std::get<gearoenix::core::event::movement::Base>(d.data);
        if (selected_widget == nullptr)
            break;
        bool widget_found = false;
        find_hited_widgets(
            data.current_position[0], data.current_position[1],
            [&](widget::Widget* const wdg, const math::Vec3& p) noexcept {
                if (selected_widget == wdg) {
                    widget_found = true;
                    selected_widget->dragged(p);
                } else if (nullptr != selected_widget) {
                    selected_widget->select_cancelled();
                    selected_widget = nullptr;
                }
            },
            [&](widget::Widget* const wdg, const math::Vec3& p, const std::vector<model::Model*>& children) noexcept {
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
