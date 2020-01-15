#include "rnd-mat-sky-equirectangular.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../system/sys-app.hpp"
#include "../buffer/rnd-buf-framed-uniform.hpp"
#include "../pipeline/rnd-pip-manager.hpp"
#include "../texture/rnd-txt-manager.hpp"
#include "../texture/rnd-txt-texture-2d.hpp"

gearoenix::render::material::SkyEquirectangular::SkyEquirectangular(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept
    : Material(Type::SkyEquirectangular, e, sizeof(Uniform))
    , color_value(math::Vec4(1.0f, 0.0f, 0.0f, 1.0f))
{
    core::sync::EndCaller<texture::Texture2D> call_txt_2d([end](const std::shared_ptr<texture::Texture2D>&) {});
    auto* const txt_mgr = e->get_system_application()->get_asset_manager()->get_texture_manager();
    color_texture = txt_mgr->get_2d(color_value.value(), call_txt_2d);
}

gearoenix::render::material::SkyEquirectangular::SkyEquirectangular(system::stream::Stream* const f, engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept
    : Material(Type::SkyEquirectangular, e, sizeof(Uniform))
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
        math::Vec4 color;
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

gearoenix::render::material::SkyEquirectangular::~SkyEquirectangular() noexcept = default;

void gearoenix::render::material::SkyEquirectangular::update() noexcept
{
    uniform_buffers->update(uniform);
}

void gearoenix::render::material::SkyEquirectangular::set_color(
    const core::Real r, const core::Real g, const core::Real b,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept
{
    core::sync::EndCaller<texture::Texture2D> call_txt_2d([end](const std::shared_ptr<texture::Texture2D>&) {});
    auto* const txt_mgr = e->get_system_application()->get_asset_manager()->get_texture_manager();
    color_texture = txt_mgr->get_2d(math::Vec3(r, g, b), call_txt_2d);
    color_value = math::Vec4(r, g, b, 1.0f);
}

void gearoenix::render::material::SkyEquirectangular::set_color(const math::Vec4& c,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept
{
    core::sync::EndCaller<texture::Texture2D> call_txt_2d([end](const std::shared_ptr<texture::Texture2D>&) {});
    auto* const txt_mgr = e->get_system_application()->get_asset_manager()->get_texture_manager();
    color_texture = txt_mgr->get_2d(c, call_txt_2d);
    color_value = c;
}

void gearoenix::render::material::SkyEquirectangular::set_color(const std::shared_ptr<texture::Texture2D>& c) noexcept
{
    color_texture = c;
}

void gearoenix::render::material::SkyEquirectangular::set_alpha(const core::Real a) noexcept
{
    uniform.alpha = a;
}
