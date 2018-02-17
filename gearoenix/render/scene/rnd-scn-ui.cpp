#include "rnd-scn-ui.hpp"
#include "../../core/event/cr-ev-bt-mouse.hpp"
#include "../../core/event/cr-ev-event.hpp"
#include "../../core/event/cr-ev-mv-mouse.hpp"
#include "../../physics/animation/phs-anm-once.hpp"
#include "../../physics/collider/phs-collider.hpp"
#include "../../physics/phs-engine.hpp"
#include "../camera/rnd-cmr-camera.hpp"
#include "../model/rnd-mdl-model.hpp"
#include "../rnd-engine.hpp"
#include "../widget/rnd-wdg-widget.hpp"
#include <limits>

gearoenix::render::scene::Ui::Ui(system::stream::Stream* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c)
    : Scene(Scene::SceneType::UI, f, e, c)
{
}

void gearoenix::render::scene::Ui::on_event(core::event::Event& e)
{
    Scene::on_event(e);
    switch (e.get_type()) {
    case core::event::Event::From::BUTTON: {
        const core::event::button::Button& be = e.to_button();
        switch (be.get_type()) {
        case core::event::button::Button::ButtonType::MOUSE: {
            const core::event::button::Mouse& mbe = be.to_mouse();
            switch (be.get_key()) {
            case core::event::button::Button::KeyType::LEFT:
                switch (be.get_action()) {
                case core::event::button::Button::ActionType::PRESS: {
                    pressed = find_widget_under_cursor(mbe.get_x(), mbe.get_y());
                    if (pressed == (core::Id)-1)
                        break;
                    const auto& id_model = all_models.find(pressed);
                    if (id_model == all_models.end())
                        break;
                    std::shared_ptr<widget::Widget> hitw;
                    if (auto hitm = id_model->second.lock()) {
                        hitw = std::static_pointer_cast<widget::Widget>(hitm);
                    } else {
                        break;
                    }
                    hitw->state_change(widget::Widget::EventType::PRESS, pressed);
                    e.take();
                    break;
                }
                case core::event::button::Button::ActionType::RELEASE: {
                    if ((core::Id)-1 == pressed)
                        break;
                    const auto& id_model = all_models.find(pressed);
                    if (id_model == all_models.end())
                        break;
                    std::shared_ptr<widget::Widget> hitw;
                    if (auto hitm = id_model->second.lock()) {
                        hitw = std::static_pointer_cast<widget::Widget>(hitm);
                    } else {
                        break;
                    }
                    hitw->state_change(widget::Widget::EventType::RELEASE, pressed);
                    pressed = (core::Id)-1;
                    e.take();
                    break;
                }
                default:
                    break;
                }
                break;
            default:
                break;
            }
            break;
        }
        default:
            break;
        }
        break;
    }
    case core::event::Event::From::MOVEMENT: {
        const core::event::movement::Movement& me = e.to_movement();
        switch (me.get_type()) {
        case core::event::movement::Movement::MovementType::MOUSE: {
            const core::event::movement::Mouse& mme = me.to_mouse();
            const core::Id hitmptr = find_widget_under_cursor(mme.get_x(), mme.get_y());
            if (mouse_overed != hitmptr && mouse_overed != (core::Id)-1) {
                const auto& id_model = all_models.find(mouse_overed);
                if (id_model == all_models.end())
                    break;
                if (auto hitm = id_model->second.lock()) {
                    std::static_pointer_cast<widget::Widget>(hitm)->state_change(widget::Widget::EventType::MOVE_OUT, mouse_overed);
                }
            }
            mouse_overed = hitmptr;
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

gearoenix::core::Id gearoenix::render::scene::Ui::find_widget_under_cursor(core::Real x, core::Real y)
{
    math::Ray3 r = cameras[curcam]->create_ray3(x, y);
    core::Real tmin = std::numeric_limits<core::Real>::max();
    core::Id hitmptr = (core::Id)-1;
    for (std::map<core::Id, std::weak_ptr<model::Model>>::iterator pidm = all_models.begin(); pidm != all_models.end();) {
        if (const std::shared_ptr<model::Model> m = pidm->second.lock()) {
            if (m->get_type() != model::Model::ModelType::WIDGET)
                continue;
            if (m->hit(r, tmin)) {
                hitmptr = pidm->first;
            }
            ++pidm;
        } else {
            pidm = all_models.erase(pidm);
        }
    }
    return hitmptr;
}
