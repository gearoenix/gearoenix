#include "rnd-wdg-button.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/event/cr-ev-button.hpp"
#include "../../physics/animation/phs-anm-animation.hpp"
#include "../../physics/phs-engine.hpp"
#include "../../physics/animation/phs-anm-manager.hpp"
#include "../../physics/collider/phs-cld-aabb.hpp"
#include "../../system/sys-app.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../model/rnd-mdl-manager.hpp"
#include "../scene/rnd-scn-scene.hpp"

constexpr gearoenix::core::Real PRESSED_SIZE = 0.75f;
constexpr gearoenix::core::Real ANIMATION_DURATION = 0.1f;

gearoenix::render::widget::Button::Button(
    const core::Id my_id,
    system::stream::Stream* const f,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Widget(my_id, Type::Button, f, e, c)
{
    set_collider(std::make_unique<physics::collider::Aabb>(math::Vec3(1.0f, 1.0f, 0.001f), math::Vec3(-1.0f, -1.0f, -0.001f)));
}

gearoenix::render::widget::Button::Button(
    const core::Id my_id,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Widget(my_id, Type::Button, e, c)
{
    set_collider(std::make_unique<physics::collider::Aabb>(math::Vec3(1.0f, 1.0f, 0.001f), math::Vec3(-1.0f, -1.0f, -0.001f)));
}

gearoenix::render::widget::Button::~Button() noexcept
{
    if (auto a = animation.lock()) a->set_activity(false);
}

void gearoenix::render::widget::Button::selected(const math::Vec3&) noexcept
{
    if (auto a = animation.lock()) a->set_activity(false);
    auto myfun = core::sync::EndCaller<model::Model>([](const std::shared_ptr<model::Model>&) {});
    auto myself = e->get_system_application()->get_asset_manager()->get_model_manager()->get_gx3d(asset_id, myfun);
    const auto a = std::make_shared<physics::animation::Animation>(
        [this, myself] (const core::Real from_start, const core::Real) noexcept {
            const auto s = 1.0f - (1.0f - PRESSED_SIZE) * (from_start / ANIMATION_DURATION);
            transformation->local_scale(s / current_scale.exchange(s));
        }, ANIMATION_DURATION,
        [this](const core::Real) noexcept {
            transformation->local_scale(PRESSED_SIZE / current_scale.exchange(PRESSED_SIZE));
        });
    e->get_physics_engine()->get_animation_manager()->add(a);
    animation = a;
}

void gearoenix::render::widget::Button::select_cancelled() noexcept
{
    if (auto a = animation.lock()) a->set_activity(false);
    auto myfun = core::sync::EndCaller<model::Model>([](const std::shared_ptr<model::Model>&) {});
    auto myself = e->get_system_application()->get_asset_manager()->get_model_manager()->get_gx3d(asset_id, myfun);
    const auto a = std::make_shared<physics::animation::Animation>(
        [this, myself](const core::Real from_start, const core::Real) noexcept {
            const auto s = PRESSED_SIZE + (1.0f - PRESSED_SIZE) * (from_start / ANIMATION_DURATION);
            transformation->local_scale(s / current_scale.exchange(s));
        }, ANIMATION_DURATION,
        [this](const core::Real) noexcept {
            transformation->local_scale(1.0f / current_scale.exchange(1.0));
        });
    e->get_physics_engine()->get_animation_manager()->add(a);
    animation = a;
}

void gearoenix::render::widget::Button::select_released() noexcept
{
    select_cancelled();
    on_click();
}

void gearoenix::render::widget::Button::set_on_click(const std::function<void()>& f) noexcept
{
    on_click = f;
}
