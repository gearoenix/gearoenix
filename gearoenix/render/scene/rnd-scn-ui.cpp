#include "rnd-scn-ui.hpp"
#include "../../core/event/cr-ev-bt-mouse.hpp"
#include "../../core/event/cr-ev-event.hpp"
#include "../../physics/animation/phs-anm-once.hpp"
#include "../../physics/collider/phs-collider.hpp"
#include "../../physics/phs-engine.hpp"
#include "../camera/rnd-cmr-camera.hpp"
#include "../model/rnd-mdl-model.hpp"
#include "../rnd-engine.hpp"
#include "../widget/rnd-wdg-widget.hpp"
#include <limits>

const std::chrono::milliseconds gearoenix::render::scene::Ui::press_animation_time = std::chrono::milliseconds(200);

gearoenix::render::scene::Ui::Ui(system::File* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c)
    : Scene(Scene::SceneType::UI, f, e, c)
{
}

std::shared_ptr<gearoenix::render::widget::Widget> gearoenix::render::scene::Ui::find_widget_under_cursor(core::Real x, core::Real y)
{
    math::Ray3 r = cameras[curcam]->create_ray3(x, y);
    core::Real tmin = std::numeric_limits<core::Real>::max();
    std::shared_ptr<model::Model> hitmptr = nullptr;
    for (std::map<core::Id, std::weak_ptr<model::Model>>::iterator pidm = all_models.begin(); pidm != all_models.end();) {
        if (const std::shared_ptr<model::Model> m = pidm->second.lock()) {
            if (m->get_type() != model::Model::ModelType::WIDGET)
                continue;
            if (m->hit(r, tmin)) {
                hitmptr = m;
            }
            ++pidm;
        } else {
            pidm = all_models.erase(pidm);
        }
    }
    return std::static_pointer_cast<widget::Widget>(hitmptr);
}

void gearoenix::render::scene::Ui::on_event(const core::event::Event& e)
{
    Scene::on_event(e);
    switch (e.get_type()) {
    case core::event::Event::EventType::BUTTON: {
        const core::event::button::Button& be = e.to_button();
        switch (be.get_type()) {
        case core::event::button::Button::ButtonType::MOUSE: {
            const core::event::button::Mouse& mbe = be.to_mouse();
            switch (be.get_key()) {
            case core::event::button::Button::KeyType::LEFT:
                switch (be.get_action()) {
                case core::event::button::Button::ActionType::PRESS: {
                    std::shared_ptr<widget::Widget> hitmptr = find_widget_under_cursor(mbe.get_x(), mbe.get_y());
                    if (nullptr != hitmptr && (hitmptr != std::get<0>(pressed) || std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - std::get<2>(pressed)) > 2 * press_animation_time)) {
                        physics::Engine* phseng = render_engine->get_physics_engine();
                        std::shared_ptr<physics::animation::Animation> anim(
                            new physics::animation::Once(
                                [hitmptr](core::Real st, core::Real) -> void {
                                    hitmptr->pop_state();
                                    hitmptr->push_state();
                                    hitmptr->local_scale(1.0f - st);
                                },
                                press_animation_time));
                        anim->set_on_delete(
                            [hitmptr, phseng]() -> void {
                                hitmptr->push_state();
                                std::shared_ptr<physics::animation::Animation> anim2(
                                    new physics::animation::Once(
                                        [hitmptr](core::Real st, core::Real) -> void {
                                            hitmptr->pop_state();
                                            hitmptr->push_state();
                                            hitmptr->local_scale(1.0f / (1.0f - st));
                                        },
                                        press_animation_time,
                                        [hitmptr]() -> void {
                                            hitmptr->pop_state();
                                            hitmptr->pop_state();
                                            hitmptr->pop_state();
                                            hitmptr->pop_state();
                                        }));
                                phseng->add_animation(anim2);
                            });
                        phseng->add_animation(anim);
                        pressed = std::make_tuple(hitmptr, anim, std::chrono::steady_clock::now());
                    }
                    break;
                }
                case core::event::button::Button::ActionType::RELEASE: {
                    std::shared_ptr<widget::Widget> hitmptr = find_widget_under_cursor(mbe.get_x(), mbe.get_y());
                    if (nullptr != hitmptr && hitmptr == std::get<0>(pressed)) {
                        pressed = std::make_tuple(hitmptr, nullptr, std::chrono::steady_clock::now() - std::chrono::duration_cast<std::chrono::steady_clock::duration>(press_animation_time));
                    }
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
    default:
        break;
    }
}
