#include "gx-rnd-wdg-button.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../../platform/stream/gx-plt-stm-path.hpp"
#include "../material/gx-rnd-mat-manager.hpp"
#include "../material/gx-rnd-mat-unlit.hpp"
#include "../mesh/gx-rnd-msh-manager.hpp"
#include "../mesh/gx-rnd-msh-mesh.hpp"
#include "../model/gx-rnd-mdl-manager.hpp"
#include "../model/gx-rnd-mdl-model.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"

void gearoenix::render::widget::Button::set_on_press_impl(const std::function<void(const math::Vec3<double>&)>& fun)
{
    on_press = [this, fun = fun](const math::Vec3<double>& p) -> void {
        if (const auto* const mdl = model_entity->get_component<model::Model>(); nullptr != mdl) {
            mdl->get_meshes().begin()->get()->get_bound_material()->set_albedo(std::shared_ptr(pressed_texture));
        }
        fun(p);
    };
}

void gearoenix::render::widget::Button::set_on_release_impl(const std::function<void(const math::Vec3<double>&)>& fun)
{
    on_release = [this, fun = fun](const math::Vec3<double>& p) -> void {
        if (const auto* const mdl = model_entity->get_component<model::Model>(); nullptr != mdl) {
            mdl->get_meshes().begin()->get()->get_bound_material()->set_albedo(std::shared_ptr(rest_texture));
        }
        fun(p);
    };
}

void gearoenix::render::widget::Button::set_on_cancel_impl(const std::function<void()>& fun)
{
    on_cancel = [this, fun = fun]() -> void {
        if (const auto* const mdl = model_entity->get_component<model::Model>(); nullptr != mdl)
            mdl->get_meshes().begin()->get()->get_bound_material()->set_albedo(std::shared_ptr(rest_texture));
        fun();
    };
}

gearoenix::render::widget::Button::Button(std::string&& name)
    : Widget(std::move(name), Type::Button)
{
    set_on_press_impl(on_press);
    set_on_release_impl(on_release);
    set_on_cancel_impl(on_cancel);
}

gearoenix::render::widget::Button::~Button() = default;

void gearoenix::render::widget::Button::construct(
    std::string&& name,
    std::string&& pressed_texture_asset,
    std::string&& rest_texture_asset,
    core::ecs::Entity* const camera_entity,
    std::shared_ptr<Widget>&& parent,
    core::job::EndCallerShared<Button>&& end_callback)
{
    const auto material_name = name + "-material";
    core::job::EndCallerShared<material::Unlit> mat_end([n = std::move(name), pt = std::move(pressed_texture_asset), r = std::move(rest_texture_asset), c = camera_entity, p = std::move(parent), e = std::move(end_callback)](std::shared_ptr<material::Unlit>&& m) mutable {
        construct(std::move(n), std::move(m), std::move(pt), std::move(r), c, std::move(p), std::move(e));
    });
    material::Manager::get().get_unlit(material_name, std::move(mat_end));
}

void gearoenix::render::widget::Button::construct(
    std::string&& name,
    std::shared_ptr<material::Material>&& mat,
    std::string&& pressed_texture_asset,
    std::string&& rest_texture_asset,
    core::ecs::Entity* const camera_entity,
    std::shared_ptr<Widget>&& parent,
    core::job::EndCallerShared<Button>&& end_callback)
{
    const auto path = platform::stream::Path::create_asset(std::move(pressed_texture_asset));
    core::job::EndCallerShared<texture::Texture2D> te([n = std::move(name), m = std::move(mat), r = std::move(rest_texture_asset), c = camera_entity, p = std::move(parent), e = std::move(end_callback)](std::shared_ptr<texture::Texture2D>&& pt) mutable {
        construct(std::move(n), std::move(m), std::move(pt), std::move(r), c, std::move(p), std::move(e));
    });
    texture::Manager::get().create_2d_from_file(path, texture::TextureInfo(), std::move(te));
}

void gearoenix::render::widget::Button::construct(
    std::string&& name,
    std::shared_ptr<material::Material>&& mat,
    std::shared_ptr<texture::Texture2D>&& pressed_texture,
    std::string&& rest_texture_asset,
    core::ecs::Entity* const camera_entity,
    std::shared_ptr<Widget>&& parent,
    core::job::EndCallerShared<Button>&& end_callback)
{
    const auto path = platform::stream::Path::create_asset(std::move(rest_texture_asset));
    core::job::EndCallerShared<texture::Texture2D> te([n = std::move(name), m = std::move(mat), pt = std::move(pressed_texture), c = camera_entity, p = std::move(parent), e = std::move(end_callback)](std::shared_ptr<texture::Texture2D>&& r) mutable {
        construct(std::move(n), std::move(m), std::move(pt), std::move(r), c, std::move(p), std::move(e));
    });
    texture::Manager::get().create_2d_from_file(path, texture::TextureInfo(), std::move(te));
}

void gearoenix::render::widget::Button::construct(
    std::string&& name,
    std::shared_ptr<material::Material>&& mat,
    std::shared_ptr<texture::Texture2D>&& pressed_texture,
    std::shared_ptr<texture::Texture2D>&& rest_texture,
    core::ecs::Entity* const camera_entity,
    std::shared_ptr<Widget>&& parent,
    core::job::EndCallerShared<Button>&& end_callback)
{
    auto copy_mat = std::shared_ptr(mat);
    mesh::Manager::get().build_plate(
        std::move(copy_mat),
        core::job::EndCallerShared<mesh::Mesh>([n = std::move(name), m = std::move(mat), pt = std::move(pressed_texture), r = std::move(rest_texture), c = camera_entity, p = std::move(parent), e = std::move(end_callback)](std::shared_ptr<mesh::Mesh>&& bm) mutable {
            construct(std::move(n), std::move(m), std::move(pt), std::move(r), std::move(bm), c, std::move(p), std::move(e));
        }));
}

void gearoenix::render::widget::Button::construct(
    std::string&& name,
    std::shared_ptr<material::Material>&& mat,
    std::shared_ptr<texture::Texture2D>&& pressed_texture,
    std::shared_ptr<texture::Texture2D>&& rest_texture,
    std::shared_ptr<mesh::Mesh>&& button_mesh,
    core::ecs::Entity* const camera_entity,
    std::shared_ptr<Widget>&& parent,
    core::job::EndCallerShared<Button>&& end_callback)
{
    mat->set_transparency(material::Transparency::Transparent);
    mat->set_albedo(std::shared_ptr(rest_texture));
    auto model_entity = model::Manager::get().build(
        name + "-model",
        parent ? parent->get_model_entity().get() : nullptr,
        { std::move(button_mesh) },
        true);
    auto but = std::make_shared<Button>(std::move(name));
    but->transform = model_entity->get_component_shared_ptr<physics::Transformation>();
    but->set_rest_texture(std::move(rest_texture));
    but->set_pressed_texture(std::move(pressed_texture));
    but->set_model_entity(std::move(model_entity));
    but->set_camera_entity(camera_entity);
    parent->add_child(but);
    but->parent = std::move(parent);
    end_callback.set_return(std::move(but));
}

void gearoenix::render::widget::Button::set_on_press(const std::function<void(const math::Vec3<double>&)>& fun)
{
    set_on_press_impl(fun);
}

void gearoenix::render::widget::Button::set_on_release(const std::function<void(const math::Vec3<double>&)>& fun)
{
    set_on_release_impl(fun);
}

void gearoenix::render::widget::Button::set_on_cancel(const std::function<void()>& fun)
{
    set_on_cancel_impl(fun);
}

void gearoenix::render::widget::Button::set_rest_texture(std::shared_ptr<texture::Texture2D>&& t)
{
    rest_texture = std::move(t);
    if (!is_pressed) {
        if (const auto* const mdl = model_entity->get_component<model::Model>(); nullptr != mdl) {
            mdl->get_meshes().begin()->get()->get_bound_material()->set_albedo(std::shared_ptr(rest_texture));
        }
    }
}

void gearoenix::render::widget::Button::set_pressed_texture(std::shared_ptr<texture::Texture2D>&& t)
{
    pressed_texture = std::move(t);
    if (is_pressed) {
        if (const auto* const mdl = model_entity->get_component<model::Model>(); nullptr != mdl) {
            mdl->get_meshes().begin()->get()->get_bound_material()->set_albedo(std::shared_ptr(pressed_texture));
        }
    }
}
