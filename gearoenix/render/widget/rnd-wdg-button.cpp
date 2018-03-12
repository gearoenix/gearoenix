#include "rnd-wdg-button.hpp"
#include "../../core/event/cr-ev-bt-button.hpp"
#include "../../physics/animation/phs-anm-once.hpp"
#include "../../physics/phs-engine.hpp"
#include "../rnd-engine.hpp"

#ifdef DEBUG_MODE
#define DEBUG_EFFECT
#endif

const gearoenix::core::Real gearoenix::render::widget::Button::press_animation_time = 0.2f;
const gearoenix::core::Real gearoenix::render::widget::Button::press_animation_time_inversed = 1.0f / gearoenix::render::widget::Button::press_animation_time;
const std::chrono::duration<gearoenix::core::Real> gearoenix::render::widget::Button::press_animation_time_duration(gearoenix::render::widget::Button::press_animation_time);
const gearoenix::core::Real gearoenix::render::widget::Button::max_scale = 1.3f;
const gearoenix::core::Real gearoenix::render::widget::Button::max_scale_reduction = gearoenix::render::widget::Button::max_scale - 1.0f;
const gearoenix::core::Real gearoenix::render::widget::Button::max_scale_inversed = 1.0f / gearoenix::render::widget::Button::max_scale;
const gearoenix::core::Real gearoenix::render::widget::Button::max_scale_inversed_reduction = 1.0f - gearoenix::render::widget::Button::max_scale_inversed;

void gearoenix::render::widget::Button::press_effect()
{
    //std::lock_guard<std::mutex> lg(effect_locker);
    switch (effect_state) {
    case EffectState::NO_ANIM: {
        effect_state = EffectState::IN_MIDDLE_OF_PRESS;
        //push_state();
        anim = std::shared_ptr<physics::animation::Animation>(
            new physics::animation::Once(
                [this](core::Real st, core::Real) -> void {
                    //std::lock_guard<std::mutex> lg(effect_locker);
                    const core::Real cursz = 1.0f - (st * press_animation_time_inversed) * max_scale_inversed_reduction;
                    const core::Real scl = cursz / current_size;
                    current_size = cursz;
#ifdef DEBUG_EFFECT
                    if (st < 0.0f || st > press_animation_time || current_size > max_scale || max_scale_inversed > current_size)
                        UNEXPECTED;
#endif
                    local_scale(scl);
                },
                std::chrono::duration_cast<std::chrono::milliseconds>(press_animation_time_duration)));
        anim->set_on_delete(
            [this]() -> void {
                physics::Engine* phseng = render_engine->get_physics_engine();
                std::shared_ptr<physics::animation::Animation> anim2(
                    new physics::animation::Once(
                        [this](core::Real st, core::Real) -> void {
                            //std::lock_guard<std::mutex> lg(effect_locker);
                            const core::Real cursz = max_scale_inversed + (st * press_animation_time_inversed) * max_scale_inversed_reduction;
                            const core::Real scl = cursz / current_size;
                            current_size = cursz;
#ifdef DEBUG_EFFECT
                            if (st < 0.0f || st > press_animation_time || current_size > max_scale || max_scale_inversed > current_size)
                                UNEXPECTED;
#endif
                            local_scale(scl);
                        },
                        std::chrono::duration_cast<std::chrono::milliseconds>(press_animation_time_duration),
                        [this]() -> void {
                            //std::lock_guard<std::mutex> lg(effect_locker);
                            //pop_state();
                            effect_state = EffectState::NO_ANIM;
                        }));
                phseng->add_animation(anim2);
                latest_anim = anim2;
            });
        physics::Engine* phseng = render_engine->get_physics_engine();
        phseng->add_animation(anim);
        break;
    }
    case EffectState::IN_MIDDLE_OF_PRESS:
        break;
    case EffectState::PRESSED:
        break;
    case EffectState::IN_MIDDLE_OF_RELEASE:
        break;
    default:
        break;
    }
}

void gearoenix::render::widget::Button::release_effect()
{
    //std::lock_guard<std::mutex> lg(effect_locker);
    switch (effect_state) {
    case EffectState::NO_ANIM:
        break;
    case EffectState::IN_MIDDLE_OF_PRESS:
        anim = nullptr;
        break;
    case EffectState::PRESSED:
        break;
    case EffectState::IN_MIDDLE_OF_RELEASE:
        break;
    default:
        break;
    }
}

void gearoenix::render::widget::Button::cancel_effect()
{
    //std::lock_guard<std::mutex> lg(effect_locker);
    switch (effect_state) {
    case EffectState::NO_ANIM:
        break;
    case EffectState::IN_MIDDLE_OF_PRESS:
        anim = nullptr;
        break;
    case EffectState::PRESSED:
        break;
    case EffectState::IN_MIDDLE_OF_RELEASE:
        break;
    default:
        break;
    }
}

gearoenix::render::widget::Button::Button(system::stream::Stream* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c)
    : Widget(f, e, c)
{
}

gearoenix::render::widget::Button::~Button()
{
    if (anim != nullptr) {
        anim->terminate();
        anim = nullptr;
    }
    if (anim = latest_anim.lock()) {
        anim->terminate();
        anim = nullptr;
    }
}
