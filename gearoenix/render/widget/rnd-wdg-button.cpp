#include "rnd-wdg-button.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../physics/animation/phs-anm-animation.hpp"
#include "../../physics/animation/phs-anm-manager.hpp"
#include "../../physics/collider/phs-cld-aabb.hpp"
#include "../../physics/phs-engine.hpp"
#include "../../system/sys-app.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../material/rnd-mat-unlit.hpp"
#include "../mesh/rnd-msh-manager.hpp"
#include "../mesh/rnd-msh-mesh.hpp"
#include "../model/rnd-mdl-manager.hpp"
#include "../model/rnd-mdl-mesh.hpp"
#include "../scene/rnd-scn-scene.hpp"
#include "rnd-wdg-text.hpp"

constexpr gearoenix::core::Real PRESSED_SIZE = 0.75f;
constexpr gearoenix::core::Real PRESSED_DELTA_SIZE = 1.0f - PRESSED_SIZE;
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
    , background_material(new material::Unlit(e, c))
{
    set_collider(std::make_unique<physics::collider::Aabb>(math::Vec3(1.0f, 1.0f, 0.001f), math::Vec3(-1.0f, -1.0f, -0.001f)));
    auto* const ast_mgr = e->get_system_application()->get_asset_manager();
    auto* const mdl_mgr = ast_mgr->get_model_manager();
    core::sync::EndCaller<Text> txt_call([c](const std::shared_ptr<Text>&) {});
    text = mdl_mgr->create<Text>(txt_call);
    text->set_text_color(theme.text_color, c);
    auto* const txt_ran = text->get_transformation();
    txt_ran->local_scale(theme.text_scale);
    txt_ran->set_location(math::Vec3(0.0f, 0.0f, 0.01f));
    add_child(text);
    background_material->set_color(0.9f, 0.075f, 0.05f, c);
    core::sync::EndCaller<mesh::Mesh> msh_call([c](const std::shared_ptr<mesh::Mesh>&) {});
    const auto plate_mesh = ast_mgr->get_mesh_manager()->create_plate(msh_call);
    add_mesh(std::make_shared<model::Mesh>(plate_mesh, background_material));
}

gearoenix::render::widget::Button::~Button() noexcept
{
    if (auto a = animation.lock())
        a->set_activity(false);
}

void gearoenix::render::widget::Button::selected(const math::Vec3&) noexcept
{
    if (auto a = animation.lock())
        a->set_activity(false);
    auto my_fun = core::sync::EndCaller<model::Model>([](const std::shared_ptr<model::Model>&) {});
    auto myself = e->get_system_application()->get_asset_manager()->get_model_manager()->get_gx3d(asset_id, my_fun);
    before_click_size = collider->get_current_local_scale()[1];
    const auto a = std::make_shared<physics::animation::Animation>(
        [this, myself, delta_size { before_click_size * PRESSED_DELTA_SIZE }](const core::Real from_start, const core::Real) noexcept {
            const auto s = before_click_size - delta_size * (from_start / ANIMATION_DURATION);
            transformation->local_scale(s / collider->get_current_local_scale()[1]);
        },
        ANIMATION_DURATION,
        [this, pressed_size { before_click_size * PRESSED_SIZE }](const core::Real) noexcept {
            transformation->local_scale(pressed_size / collider->get_current_local_scale()[1]);
        });
    e->get_physics_engine()->get_animation_manager()->add(a);
    animation = a;
}

void gearoenix::render::widget::Button::select_cancelled() noexcept
{
    if (auto a = animation.lock())
        a->set_activity(false);
    auto my_fun = core::sync::EndCaller<model::Model>([](const std::shared_ptr<model::Model>&) {});
    auto myself = e->get_system_application()->get_asset_manager()->get_model_manager()->get_gx3d(asset_id, my_fun);
    const auto a = std::make_shared<physics::animation::Animation>(
        [this, myself, pressed_size { before_click_size * PRESSED_SIZE }, delta_size { before_click_size * PRESSED_DELTA_SIZE }](const core::Real from_start, const core::Real) noexcept {
            const auto s = pressed_size + delta_size * (from_start / ANIMATION_DURATION);
            transformation->local_scale(s / collider->get_current_local_scale()[1]);
        },
        ANIMATION_DURATION,
        [this](const core::Real) noexcept {
            transformation->local_scale(before_click_size / collider->get_current_local_scale()[1]);
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

void gearoenix::render::widget::Button::set_text(const std::wstring& t, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    text->set_text(t, c);
    const auto scale = text->get_collider()->get_current_local_scale()[0] / (theme.text_scale * collider->get_current_local_scale()[0]);
    transformation->local_x_scale(scale);
    text->get_transformation()->local_x_scale(1.0f / scale);
}
