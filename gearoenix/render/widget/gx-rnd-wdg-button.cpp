#include "gx-rnd-wdg-button.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../platform/stream/gx-plt-stm-path.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../material/gx-rnd-mat-manager.hpp"
#include "../material/gx-rnd-mat-unlit.hpp"
#include "../mesh/gx-rnd-msh-manager.hpp"
#include "../model/gx-rnd-mdl-builder.hpp"
#include "../model/gx-rnd-mdl-manager.hpp"
#include "../model/gx-rnd-mdl-model.hpp"
#include "../scene/gx-rnd-scn-builder.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"

void gearoenix::render::widget::Button::set_on_press_impl(const std::function<void(const math::Vec3<double>&)>& fun) noexcept
{
    on_press = [this, fun = fun](const math::Vec3<double>& p) noexcept -> void {
        if (auto* const mdl = e.get_world()->get_component<model::Model>(model_entity_id); nullptr != mdl)
            mdl->get_bound_material()->set_albedo(pressed_texture);
        fun(p);
    };
}

void gearoenix::render::widget::Button::set_on_release_impl(const std::function<void(const math::Vec3<double>&)>& fun) noexcept
{
    on_release = [this, fun = fun](const math::Vec3<double>& p) noexcept -> void {
        if (auto* const mdl = e.get_world()->get_component<model::Model>(model_entity_id); nullptr != mdl)
            mdl->get_bound_material()->set_albedo(rest_texture);
        fun(p);
    };
}

void gearoenix::render::widget::Button::set_on_cancel_impl(const std::function<void()>& fun) noexcept
{
    on_cancel = [this, fun = fun]() noexcept -> void {
        if (auto* const mdl = e.get_world()->get_component<model::Model>(model_entity_id); nullptr != mdl)
            mdl->get_bound_material()->set_albedo(rest_texture);
        fun();
    };
}

gearoenix::render::widget::Button::Button(const std::string& name, engine::Engine& e) noexcept
    : Widget(name, Type::Button, e)
{
    set_on_press_impl(on_press);
    set_on_release_impl(on_release);
    set_on_cancel_impl(on_cancel);
}

gearoenix::render::widget::Button::~Button() noexcept = default;

std::pair<std::shared_ptr<gearoenix::render::model::Builder>, std::shared_ptr<gearoenix::render::widget::Button>>
gearoenix::render::widget::Button::construct(
    const std::string& name,
    engine::Engine& e,
    const std::string& pressed_texture_asset,
    const std::string& rest_texture_asset,
    const core::ecs::entity_id_t camera_id,
    Widget& parent,
    scene::Builder& scene_builder,
    const core::sync::EndCaller& end_callback) noexcept
{
    auto mat = e.get_material_manager()->get_unlit(name + "-material", end_callback);
    mat->set_transparency(render::material::Transparency::Transparent);
    auto rest_txt = e.get_texture_manager()->create_2d_from_file(
        rest_texture_asset,
        platform::stream::Path::create_asset(rest_texture_asset),
        render::texture::TextureInfo(),
        end_callback);
    auto pressed_txt = e.get_texture_manager()->create_2d_from_file(
        pressed_texture_asset,
        platform::stream::Path::create_asset(pressed_texture_asset),
        render::texture::TextureInfo(),
        end_callback);
    mat->set_albedo(rest_txt);
    auto model_builder = e.get_model_manager()->build(
        name + "-model",
        e.get_mesh_manager()->build_plate(end_callback),
        std::move(mat),
        core::sync::EndCaller(end_callback),
        true);
    const auto id = model_builder->get_id();
    auto but = std::make_shared<render::widget::Button>(name, e);
    but->set_rest_texture(rest_txt);
    but->set_pressed_texture(pressed_txt);
    but->set_model_entity_id(id);
    but->set_camera_entity_id(camera_id);
    parent.add_child(but);
    scene_builder.add(std::shared_ptr(model_builder));
    return { std::move(model_builder), std::move(but) };
}

void gearoenix::render::widget::Button::set_on_press(const std::function<void(const math::Vec3<double>&)>& fun) noexcept
{
    set_on_press_impl(fun);
}

void gearoenix::render::widget::Button::set_on_release(const std::function<void(const math::Vec3<double>&)>& fun) noexcept
{
    set_on_release_impl(fun);
}

void gearoenix::render::widget::Button::set_on_cancel(const std::function<void()>& fun) noexcept
{
    set_on_cancel_impl(fun);
}
