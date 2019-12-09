#include "rnd-mat-material.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../system/sys-app.hpp"
#include "../pipeline/rnd-pip-manager.hpp"
#include "../texture/rnd-txt-manager.hpp"

gearoenix::render::material::Material::Material(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept
    : uniform_buffers(new buffer::FramedUniform(sizeof(Uniform), e))
    , color_value(math::Vec4(1.0f, 0.0f, 0.0f, 1.0f))
    , emission_value(math::Vec3(0.0f, 0.0f, 0.0f))
    , metallic_roughness_value(math::Vec2(0.5f, 0.5f))
    , normal_value(math::Vec3(0.5f, 0.5f, 1.0f))
    , e(e)
{
    core::sync::EndCaller<texture::Texture2D> calltxt2d([end](const std::shared_ptr<texture::Texture2D>&) {});
    auto* const txtmgr = e->get_system_application()->get_asset_manager()->get_texture_manager();
    color_texture = txtmgr->get_2d(color_value.value(), calltxt2d);
    emission_texture = txtmgr->get_2d(emission_value.value(), calltxt2d);
    metallic_roughness_texture = txtmgr->get_2d(metallic_roughness_value.value(), calltxt2d);
    normal_texture = txtmgr->get_2d(normal_value.value(), calltxt2d);
}

gearoenix::render::material::Material::Material(system::stream::Stream* const f, engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept
    : uniform_buffers(new buffer::FramedUniform(sizeof(Uniform), e))
    , e(e)
{
    auto* const txtmgr = e->get_system_application()->get_asset_manager()->get_texture_manager();

    // Reading alpha
    if (f->read_bool()) {
        uniform.alpha = 1.0f;
    } else {
        f->read(uniform.alpha);
    }
    // Reading color
    if (f->read_bool()) {
        core::sync::EndCaller<texture::Texture> tcall([end](const std::shared_ptr<texture::Texture>&) {});
        color_texture = std::dynamic_pointer_cast<texture::Texture2D>(txtmgr->get_gx3d(f->read<core::Id>(), tcall));
    } else {
        core::sync::EndCaller<texture::Texture2D> tcall([end](const std::shared_ptr<texture::Texture2D>&) {});
        math::Vec4 color;
        color.read(f);
        color_texture = std::dynamic_pointer_cast<texture::Texture2D>(txtmgr->get_2d(color, tcall));
        color_value = color;
    }
    // Reading emission
    if (f->read_bool()) {
        core::sync::EndCaller<texture::Texture> tcall([end](const std::shared_ptr<texture::Texture>&) {});
        emission_texture = std::dynamic_pointer_cast<texture::Texture2D>(txtmgr->get_gx3d(f->read<core::Id>(), tcall));
    } else {
        core::sync::EndCaller<texture::Texture2D> tcall([end](const std::shared_ptr<texture::Texture2D>&) {});
        math::Vec3 emission;
        emission.read(f);
        emission_texture = std::dynamic_pointer_cast<texture::Texture2D>(txtmgr->get_2d(emission, tcall));
        emission_value = emission;
    }
    // Reading metallic_roughness
    if (f->read_bool()) {
        core::sync::EndCaller<texture::Texture> tcall([end](const std::shared_ptr<texture::Texture>&) {});
        metallic_roughness_texture = std::dynamic_pointer_cast<texture::Texture2D>(txtmgr->get_gx3d(f->read<core::Id>(), tcall));
    } else {
        core::sync::EndCaller<texture::Texture2D> tcall([end](const std::shared_ptr<texture::Texture2D>&) {});
        math::Vec2 metallic_roughness;
        metallic_roughness.read(f);
        metallic_roughness_texture = std::dynamic_pointer_cast<texture::Texture2D>(txtmgr->get_2d(metallic_roughness, tcall));
        metallic_roughness_value = metallic_roughness;
    }
    // Reading normal
    if (f->read_bool()) {
        core::sync::EndCaller<texture::Texture> tcall([end](const std::shared_ptr<texture::Texture>&) {});
        normal_texture = std::dynamic_pointer_cast<texture::Texture2D>(txtmgr->get_gx3d(f->read<core::Id>(), tcall));
    } else {
        core::sync::EndCaller<texture::Texture2D> tcall([end](const std::shared_ptr<texture::Texture2D>&) {});
        normal_value = math::Vec3(0.5f, 0.5f, 1.0f);
        normal_texture = std::dynamic_pointer_cast<texture::Texture2D>(txtmgr->get_2d(normal_value.value(), tcall));
    }
    // Translucency
    if (f->read_bool())
        translucency = TranslucencyMode::Transparent;
    is_shadow_caster = f->read_bool();
    f->read(uniform.alpha_cutoff);
}

void gearoenix::render::material::Material::update_uniform() noexcept
{
    uniform_buffers->update(uniform);
}

void gearoenix::render::material::Material::set_metallic_factor(const core::Real f) noexcept
{
    uniform.metallic_factor = f;
}

void gearoenix::render::material::Material::set_roughness_factor(const core::Real f) noexcept
{
    uniform.roughness_factor = f;
}

void gearoenix::render::material::Material::set_color(
    const core::Real r, const core::Real g, const core::Real b,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept
{
    core::sync::EndCaller<texture::Texture2D> calltxt2d([end](const std::shared_ptr<texture::Texture2D>&) {});
    auto* const txtmgr = e->get_system_application()->get_asset_manager()->get_texture_manager();
    color_texture = txtmgr->get_2d(math::Vec3(r, g, b), calltxt2d);
    color_value = math::Vec4(r, g, b, 1.0f);
}

void gearoenix::render::material::Material::set_color(const std::shared_ptr<texture::Texture2D>& c) noexcept
{
    color_texture = c;
}

void gearoenix::render::material::Material::set_alpha(const core::Real a) noexcept
{
    uniform.alpha = a;
}
