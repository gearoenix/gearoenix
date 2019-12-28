#include "rnd-mat-unlit.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../system/sys-app.hpp"
#include "../buffer/rnd-buf-framed-uniform.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../pipeline/rnd-pip-manager.hpp"
#include "../texture/rnd-txt-manager.hpp"

gearoenix::render::material::Unlit::Unlit(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept
    : Material(Type::Unlit, e, sizeof(Uniform))
    , color_value(math::Vec4(1.0f, 0.0f, 0.0f, 1.0f))
{
    core::sync::EndCaller<texture::Texture2D> call_txt_2d([end](const std::shared_ptr<texture::Texture2D>&) {});
    auto* const txt_mgr = e->get_system_application()->get_asset_manager()->get_texture_manager();
    color_texture = txt_mgr->get_2d(color_value.value(), call_txt_2d);
}

gearoenix::render::material::Unlit::Unlit(system::stream::Stream* const f, engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept
    : Material(Type::Unlit, e, sizeof(Uniform))
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
    // Translucency
    if (f->read_bool())
        translucency = TranslucencyMode::Transparent;
    is_shadow_caster = f->read_bool();
    f->read(uniform.alpha_cutoff);
}

void gearoenix::render::material::Unlit::update() noexcept
{
    uniform_buffers->update(uniform);
}

void gearoenix::render::material::Unlit::set_color(
    const core::Real r, const core::Real g, const core::Real b,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept
{
    core::sync::EndCaller<texture::Texture2D> calltxt2d([end](const std::shared_ptr<texture::Texture2D>&) {});
    auto* const txtmgr = e->get_system_application()->get_asset_manager()->get_texture_manager();
    color_texture = txtmgr->get_2d(math::Vec3(r, g, b), calltxt2d);
    color_value = math::Vec4(r, g, b, 1.0f);
}

void gearoenix::render::material::Unlit::set_color(const math::Vec4& c,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept
{
    core::sync::EndCaller<texture::Texture2D> calltxt2d([end](const std::shared_ptr<texture::Texture2D>&) {});
    auto* const txtmgr = e->get_system_application()->get_asset_manager()->get_texture_manager();
    color_texture = txtmgr->get_2d(c, calltxt2d);
    color_value = c;
}

void gearoenix::render::material::Unlit::set_color(const std::shared_ptr<texture::Texture2D>& c) noexcept
{
    color_texture = c;
}

void gearoenix::render::material::Unlit::set_alpha(const core::Real a) noexcept
{
    uniform.alpha = a;
}