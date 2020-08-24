#include "gx-demo-wwr-rotating-button.hpp"
#include <gearoenix/render/widget/rnd-wdg-button.hpp>
#include <gearoenix/render/model/rnd-mdl-transformation.hpp>
#include <gearoenix/render/engine/rnd-eng-engine.hpp>
#include <gearoenix/physics/phs-engine.hpp>
#include <gearoenix/physics/animation/phs-anm-manager.hpp>
#include <gearoenix/physics/animation/phs-anm-animation.hpp>

gearoenix::demo::wwr::RotatingButton::RotatingButton(std::shared_ptr<render::widget::Button> b) noexcept
    : button(std::move(b)) {
    bool odd = true;
    auto*const e = button->get_e();
    auto*const anm_mgr = e->get_physics_engine()->get_animation_manager();
    for(const auto& c: button->get_children()) {
        const auto &child = c.second;
        if(child->get_model_type() == render::model::Type::Dynamic) {
            std::shared_ptr<physics::animation::Animation> a(new physics::animation::Animation(
                    child->get_name() + "-rot-anm",
                    [odd { odd }, rotation_speed { rotation_speed }, child] (double, double delta_time) {
                        if(odd) delta_time = -delta_time;
                        child->get_transformation()->local_z_rotate(*rotation_speed * delta_time);
                        },
                    std::numeric_limits<double>::max()
                    ));
            anm_mgr->add(button->get_root()->get_id(), a);
            animations.push_back(std::move(a));
            odd = !odd;
        }
    }
}

gearoenix::demo::wwr::RotatingButton::~RotatingButton() noexcept {
    for(const auto &a: animations) a->set_activity(false);
}
