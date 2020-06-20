#include "rnd-mat-pbr.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../system/sys-app.hpp"
#include "../buffer/rnd-buf-framed-uniform.hpp"
#include "../texture/rnd-txt-manager.hpp"
#include "../texture/rnd-txt-texture-2d.hpp"
#include "../texture/rnd-txt-texture-cube.hpp"

gearoenix::render::material::Pbr::Pbr(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept
    : Material(Type::Pbr, e, sizeof(Uniform))
    , color_value(math::Vec4(1.0f, 0.0f, 0.0f, 1.0f))
    , emission_value(math::Vec3(0.0f, 0.0f, 0.0f))
    , metallic_roughness_value(math::Vec2(1.0f, 1.0f))
    , normal_value(math::Vec3(0.5f, 0.5f, 1.0f))
{
    core::sync::EndCaller<texture::Texture2D> call_txt_2d([end](const std::shared_ptr<texture::Texture2D>&) {});
    core::sync::EndCaller<texture::TextureCube> call_txt_cube([end](const std::shared_ptr<texture::TextureCube>&) {});
    auto* const txt_mgr = e->get_system_application()->get_asset_manager()->get_texture_manager();
    color_texture = txt_mgr->get_2d(color_value.value(), call_txt_2d);
    emission_texture = txt_mgr->get_2d(emission_value.value(), call_txt_2d);
    metallic_roughness_texture = txt_mgr->get_2d(metallic_roughness_value.value(), call_txt_2d);
    normal_texture = txt_mgr->get_2d(normal_value.value(), call_txt_2d);
    irradiance = radiance = txt_mgr->get_cube_zero_3c(call_txt_cube).get();
}

gearoenix::render::material::Pbr::Pbr(system::stream::Stream* const f, engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept
    : Material(Type::Pbr, e, sizeof(Uniform))
{
    auto* const txt_mgr = e->get_system_application()->get_asset_manager()->get_texture_manager();
    uniform.alpha = read_alpha(f);
    std::tie(color_texture, color_value) = read_t2d_v4(f, end);
    if (f->read_bool())
        translucency = TranslucencyMode::Transparent;
    is_shadow_caster = f->read_bool();
    f->read(uniform.alpha_cutoff);
    std::tie(emission_texture, emission_value) = read_t2d_v3(f, end);
    std::tie(metallic_roughness_texture, metallic_roughness_value) = read_t2d_v2(f, end);
    // Reading normal
    if (f->read_bool()) {
        core::sync::EndCaller<texture::Texture> txt_call([end](const std::shared_ptr<texture::Texture>&) {});
        normal_texture = std::dynamic_pointer_cast<texture::Texture2D>(txt_mgr->get_gx3d(f->read<core::Id>(), txt_call));
    } else {
        core::sync::EndCaller<texture::Texture2D> txt_call([end](const std::shared_ptr<texture::Texture2D>&) {});
        normal_value = math::Vec3(0.5f, 0.5f, 1.0f);
        normal_texture = std::dynamic_pointer_cast<texture::Texture2D>(txt_mgr->get_2d(normal_value.value(), txt_call));
    }
    core::sync::EndCaller<texture::TextureCube> call_txt_cube([end](const std::shared_ptr<texture::TextureCube>&) {});
    irradiance = radiance = txt_mgr->get_cube_zero_3c(call_txt_cube).get();
}

gearoenix::render::material::Pbr::Pbr(const Pbr& o) noexcept
    : Material(o)
    , color_texture(o.color_texture)
    , color_value(o.color_value)
    , emission_texture(o.emission_texture)
    , emission_value(o.emission_value)
    , metallic_roughness_texture(o.metallic_roughness_texture)
    , metallic_roughness_value(o.metallic_roughness_value)
    , normal_texture(o.normal_texture)
    , normal_value(o.normal_value)
    , irradiance(o.irradiance)
    , radiance(o.radiance)
    , uniform(o.uniform)
{}

gearoenix::render::material::Pbr::~Pbr() noexcept = default;

void gearoenix::render::material::Pbr::Pbr::update() noexcept
{
    uniform_buffers->update(uniform);
}

gearoenix::render::material::Material* gearoenix::render::material::Pbr::clone() const noexcept
{
    return new Pbr(*this);
}

void gearoenix::render::material::Pbr::set_metallic_factor(const float f) noexcept
{
    uniform.metallic_factor = f;
}

void gearoenix::render::material::Pbr::set_roughness_factor(const float f) noexcept
{
    uniform.roughness_factor = f;
}

void gearoenix::render::material::Pbr::set_color(
    const std::uint32_t code, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept
{
    set_color(math::Vec4(
                  float(code >> 24U) / 255.0f,
                  float((code >> 16U) & 255U) / 255.0f,
                  float((code >> 8U) & 255U) / 255.0f,
                  float(code & 255U) / 255.0f),
        end);
}

void gearoenix::render::material::Pbr::set_color(
    const float r, const float g, const float b,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept
{
    set_color(math::Vec4(r, g, b, 1.0f), end);
}

void gearoenix::render::material::Pbr::set_color(const math::Vec4<float>& c,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept
{
    core::sync::EndCaller<texture::Texture2D> call_txt_2d([end](const std::shared_ptr<texture::Texture2D>&) {});
    auto* const txt_mgr = e->get_system_application()->get_asset_manager()->get_texture_manager();
    color_texture = txt_mgr->get_2d(c, call_txt_2d);
    color_value = c;
}

void gearoenix::render::material::Pbr::set_color(const std::shared_ptr<texture::Texture2D>& c) noexcept
{
    color_texture = c;
    color_value = std::nullopt;
}

void gearoenix::render::material::Pbr::set_alpha(const float a) noexcept
{
    uniform.alpha = a;
}

void gearoenix::render::material::Pbr::set_radiance(gearoenix::render::texture::TextureCube* const t) noexcept
{
    radiance = t;
    uniform.radiance_lod_coefficient = static_cast<float>(
        engine::Configuration::compute_runtime_reflection_radiance_levels(
            static_cast<int>(t->get_aspect() - 1)));
}
