#include "gx-rnd-mat-skybox-equirectangular.hpp"
#include "../../core/asset/gx-cr-asset-manager.hpp"
#include "../../system/gx-sys-app.hpp"
#include "../buffer/gx-rnd-buf-framed-uniform.hpp"
#include "../pipeline/gx-rnd-pip-manager.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"

gearoenix::render::material::SkyboxEquirectangular::SkyboxEquirectangular(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept
    : Material(Type::SkyboxEquirectangular, e, sizeof(Uniform))
    , color_value(math::Vec4(0.0f, 0.0f, 0.0f, 1.0f))
{
    core::sync::EndCaller<texture::Texture2D> call_txt_2d([end](const std::shared_ptr<texture::Texture2D>&) {});
    auto* const txt_mgr = e->get_system_application()->get_asset_manager()->get_texture_manager();
    color_texture = txt_mgr->get_2d(color_value.value(), call_txt_2d);
}

gearoenix::render::material::SkyboxEquirectangular::SkyboxEquirectangular(system::stream::Stream* const f, engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept
    : Material(Type::SkyboxEquirectangular, e, sizeof(Uniform))
{
    auto* const txt_mgr = e->get_system_application()->get_asset_manager()->get_texture_manager();

    // Reading alpha
    if (f->read_bool()) {
        uniform.alpha = 1.0f;
    } else {
        f->read(uniform.alpha);
    }
    // Reading color
    if (f->read_bool()) {
        core::sync::EndCaller<texture::Texture> txt_call([end](const std::shared_ptr<texture::Texture>&) {});
        color_texture = std::dynamic_pointer_cast<texture::Texture2D>(txt_mgr->get_gx3d(f->read<core::Id>(), txt_call));
    } else {
        core::sync::EndCaller<texture::Texture2D> txt_call([end](const std::shared_ptr<texture::Texture2D>&) {});
        math::Vec4<float> color;
        color.read(f);
        color_texture = std::dynamic_pointer_cast<texture::Texture2D>(txt_mgr->get_2d(color, txt_call));
        color_value = color;
    }
    // Translucency
    if (f->read_bool())
        translucency = TranslucencyMode::Transparent;
    is_shadow_caster = f->read_bool();
    f->read(uniform.alpha_cutoff);
}

gearoenix::render::material::SkyboxEquirectangular::~SkyboxEquirectangular() noexcept = default;

void gearoenix::render::material::SkyboxEquirectangular::update() noexcept
{
    uniform_buffers->update(uniform);
}

void gearoenix::render::material::SkyboxEquirectangular::set_color(
    const float r, const float g, const float b,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept
{
    core::sync::EndCaller<texture::Texture2D> call_txt_2d([end](const std::shared_ptr<texture::Texture2D>&) {});
    auto* const txt_mgr = e->get_system_application()->get_asset_manager()->get_texture_manager();
    color_texture = txt_mgr->get_2d(math::Vec3(r, g, b), call_txt_2d);
    color_value = math::Vec4(r, g, b, 1.0f);
}

void gearoenix::render::material::SkyboxEquirectangular::set_color(const math::Vec4<float>& c,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept
{
    core::sync::EndCaller<texture::Texture2D> call_txt_2d([end](const std::shared_ptr<texture::Texture2D>&) {});
    auto* const txt_mgr = e->get_system_application()->get_asset_manager()->get_texture_manager();
    color_texture = txt_mgr->get_2d(c, call_txt_2d);
    color_value = c;
}

void gearoenix::render::material::SkyboxEquirectangular::set_color(const std::shared_ptr<texture::Texture2D>& c) noexcept
{
    color_texture = c;
}

void gearoenix::render::material::SkyboxEquirectangular::set_alpha(const float a) noexcept
{
    uniform.alpha = a;
}
