#include "rnd-mat-skybox-cube.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../system/sys-app.hpp"
#include "../buffer/rnd-buf-framed-uniform.hpp"
#include "../pipeline/rnd-pip-manager.hpp"
#include "../texture/rnd-txt-manager.hpp"
#include "../texture/rnd-txt-texture-cube.hpp"

gearoenix::render::material::SkyboxCube::SkyboxCube(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept
    : Material(Type::SkyboxCube, e, sizeof(Uniform))
    , color_value(math::Vec4(1.0f, 0.0f, 0.0f, 1.0f))
{
    core::sync::EndCaller<texture::TextureCube> call_txt_2d([end](const std::shared_ptr<texture::TextureCube>&) {});
    auto* const txt_mgr = e->get_system_application()->get_asset_manager()->get_texture_manager();
    color_texture = txt_mgr->get_cube(color_value.value(), call_txt_2d);
}

gearoenix::render::material::SkyboxCube::SkyboxCube(system::stream::Stream* const f, engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept
    : Material(Type::SkyboxCube, e, sizeof(Uniform))
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
        color_texture = std::dynamic_pointer_cast<texture::TextureCube>(txt_mgr->get_gx3d(f->read<core::Id>(), txt_call));
    } else {
        core::sync::EndCaller<texture::TextureCube> txt_call([end](const std::shared_ptr<texture::TextureCube>&) {});
        math::Vec4 color;
        color.read(f);
        color_texture = txt_mgr->get_cube(color, txt_call);
        color_value = color;
    }
    // Translucency
    if (f->read_bool())
        translucency = TranslucencyMode::Transparent;
    is_shadow_caster = f->read_bool();
    f->read(uniform.alpha_cutoff);
}

gearoenix::render::material::SkyboxCube::~SkyboxCube() noexcept = default;

void gearoenix::render::material::SkyboxCube::update() noexcept
{
    uniform_buffers->update(uniform);
}

void gearoenix::render::material::SkyboxCube::set_color(
    const core::Real r, const core::Real g, const core::Real b,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept
{
    core::sync::EndCaller<texture::TextureCube> call_txt([end](const std::shared_ptr<texture::TextureCube>&) {});
    auto* const txt_mgr = e->get_system_application()->get_asset_manager()->get_texture_manager();
    color_texture = txt_mgr->get_cube(math::Vec3(r, g, b), call_txt);
    color_value = math::Vec4(r, g, b, 1.0f);
}

void gearoenix::render::material::SkyboxCube::set_color(const math::Vec4& c,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept
{
    core::sync::EndCaller<texture::TextureCube> call_txt([end](const std::shared_ptr<texture::TextureCube>&) {});
    auto* const txt_mgr = e->get_system_application()->get_asset_manager()->get_texture_manager();
    color_texture = txt_mgr->get_cube(c, call_txt);
    color_value = c;
}

void gearoenix::render::material::SkyboxCube::set_color(const std::shared_ptr<texture::TextureCube>& c) noexcept
{
    color_texture = c;
}

void gearoenix::render::material::SkyboxCube::set_alpha(const core::Real a) noexcept
{
    uniform.alpha = a;
}
