#include "gx-rnd-mat-unlit.hpp"
#include "../../core/asset/gx-cr-asset-manager.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../buffer/gx-rnd-buf-framed-uniform.hpp"
#include "../pipeline/gx-rnd-pip-manager.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"

gearoenix::render::material::Unlit::Unlit(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept
    : Material(Type::Unlit, e, sizeof(Uniform))
    , color_value(math::Vec4(1.0f, 0.0f, 0.0f, 1.0f))
{
    core::sync::EndCaller<texture::Texture2D> call_txt_2d([end](const std::shared_ptr<texture::Texture2D>&) {});
    auto* const txt_mgr = e->get_platform_application()->get_asset_manager()->get_texture_manager();
    color_texture = txt_mgr->get_2d(color_value.value(), call_txt_2d);
}

gearoenix::render::material::Unlit::Unlit(platform::stream::Stream* const f, engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept
    : Material(Type::Unlit, e, sizeof(Uniform))
{
    uniform.alpha = read_alpha(f);
    std::tie(color_texture, color_value) = read_t2d_v4(f, end);
    if (f->read_bool())
        translucency = TranslucencyMode::Transparent;
    is_shadow_caster = f->read_bool();
    f->read(uniform.alpha_cutoff);
}

gearoenix::render::material::Unlit::~Unlit() noexcept = default;

void gearoenix::render::material::Unlit::update() noexcept
{
    uniform_buffers->update(uniform);
}

void gearoenix::render::material::Unlit::set_color(
    const float r, const float g, const float b,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept
{
    core::sync::EndCaller<texture::Texture2D> call_txt_2d([end](const std::shared_ptr<texture::Texture2D>&) {});
    auto* const txt_mgr = e->get_platform_application()->get_asset_manager()->get_texture_manager();
    e->late_delete(std::move(color_texture));
    color_texture = txt_mgr->get_2d(math::Vec3(r, g, b), call_txt_2d);
    color_value = math::Vec4(r, g, b, 1.0f);
}

void gearoenix::render::material::Unlit::set_color(const math::Vec4<float>& c,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept
{
    core::sync::EndCaller<texture::Texture2D> call_txt_2d([end](const std::shared_ptr<texture::Texture2D>&) {});
    auto* const txt_mgr = e->get_platform_application()->get_asset_manager()->get_texture_manager();
    e->late_delete(std::move(color_texture));
    color_texture = txt_mgr->get_2d(c, call_txt_2d);
    color_value = c;
}

void gearoenix::render::material::Unlit::set_color(const std::shared_ptr<texture::Texture2D>& c) noexcept
{
    e->late_delete(std::move(color_texture));
    color_texture = c;
}

void gearoenix::render::material::Unlit::set_alpha(const float a) noexcept
{
    uniform.alpha = a;
}
