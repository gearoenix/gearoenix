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

gearoenix::render::widget::Button::Button(
    const core::Id my_id,
    system::stream::Stream* const f,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Widget(my_id, Type::Button, f, e, c)
{
    set_collider(std::make_unique<physics::collider::Aabb>(*collider));
}

gearoenix::render::widget::Button::Button(
    const core::Id my_id,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Widget(my_id, Type::Button, e, c)
    , background_material(new material::Unlit(e, c))
{
    set_collider(std::make_unique<physics::collider::Aabb>(math::Vec3(1.0, 1.0, 0.001), math::Vec3(-1.0, -1.0, -0.001)));
    auto* const ast_mgr = e->get_system_application()->get_asset_manager();
    auto* const mdl_mgr = ast_mgr->get_model_manager();
    core::sync::EndCaller<Text> txt_call([c](const std::shared_ptr<Text>&) {});
    text = mdl_mgr->create<Text>(txt_call);
    text->set_text_color(theme.text_color, c);
    auto* const txt_ran = text->get_transformation();
    txt_ran->local_scale(theme.text_scale);
    txt_ran->set_location(math::Vec3(0.0, 0.0, 0.01));
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

void gearoenix::render::widget::Button::selected(const math::Vec3<double>&) noexcept
{
    if (auto a = animation.lock())
        a->set_activity(false);
    auto end = core::sync::EndCaller<model::Model>([](const std::shared_ptr<model::Model>&) {});
    auto myself = e->get_system_application()->get_asset_manager()->get_model_manager()->get_gx3d(asset_id, end);
    const auto a = std::make_shared<physics::animation::Animation>(
        [this, myself](const double from_start, const double) noexcept {
            const auto s = 1.0 - (1.0 - pressed_size) * from_start / animation_duration;
            transformation->local_scale(s / scale_down_progress);
            scale_down_progress = s;
        },
        animation_duration,
        [this](const double) noexcept {
            transformation->local_scale(pressed_size / scale_down_progress);
            scale_down_progress = pressed_size;
        });
    e->get_physics_engine()->get_animation_manager()->add(get_root()->get_asset_id(), a);
    animation = a;
}

void gearoenix::render::widget::Button::select_cancelled() noexcept
{
    if (auto a = animation.lock())
        a->set_activity(false);
    auto my_fun = core::sync::EndCaller<model::Model>([](const std::shared_ptr<model::Model>&) {});
    auto myself = e->get_system_application()->get_asset_manager()->get_model_manager()->get_gx3d(asset_id, my_fun);
    const auto a = std::make_shared<physics::animation::Animation>(
        [this, myself](const double from_start, const double) noexcept {
            const auto s = pressed_size + (1.0 - scale_down_progress) * (from_start / animation_duration);
            transformation->local_scale(s / scale_down_progress);
            scale_down_progress = s;
        },
        animation_duration,
        [this](const double) noexcept {
            transformation->local_scale(1.0 / scale_down_progress);
            scale_down_progress = 1.0;
        });
    e->get_physics_engine()->get_animation_manager()->add(get_root()->get_asset_id(), a);
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
