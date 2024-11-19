#include "gx-rnd-mat-pbr.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"

gearoenix::render::material::Pbr::Pbr(engine::Engine& e, const std::string& name)
    : Material(e, name, Id::Pbr)
    , albedo_factor(1.0f)
    , emission_roughness_factor(1.0f)
    , normal_metallic_factor(1.0f)
    , alpha_cutoff_occlusion_strength_reserved_reserved(math::Numeric::epsilon<float>, 1.0f, 0.0f, 0.0f)
{
}

void gearoenix::render::material::Pbr::initialise(core::job::EndCallerShared<Pbr>&& c)
{
    const auto& tm = e.get_texture_manager();
    tm->create_2d_from_colour(
        math::Vec4(1.0f, 1.0f, 1.0f, 1.0f),
        core::job::EndCallerShared<texture::Texture2D>([this, c](std::shared_ptr<texture::Texture2D>&& t) {
            set_albedo(std::move(t));
            (void)c; // prevent this from deletion
        }));
    tm->create_2d_from_colour(
        math::Vec4(0.5f, 0.5f, 1.0f, 0.0f),
        core::job::EndCallerShared<texture::Texture2D>([this, c](std::shared_ptr<texture::Texture2D>&& t) {
            set_normal(std::move(t));
            (void)c; // prevent this from deletion
        }));
    tm->create_2d_from_colour(
        math::Vec4(0.0f, 0.0f, 0.0f, 0.0f),
        core::job::EndCallerShared<texture::Texture2D>([this, c](std::shared_ptr<texture::Texture2D>&& t) {
            set_emission(std::move(t));
            (void)c; // prevent this from deletion
        }));
    tm->create_2d_from_colour(
        math::Vec4(1.0f, 1.0f, 0.0f, 0.0f),
        core::job::EndCallerShared<texture::Texture2D>([this, c](std::shared_ptr<texture::Texture2D>&& t) {
            set_metallic_roughness(std::move(t));
            (void)c; // prevent this from deletion
        }));
    tm->create_2d_from_colour(
        math::Vec4(1.0f, 1.0f, 1.0f, 1.0f),
        core::job::EndCallerShared<texture::Texture2D>([this, c](std::shared_ptr<texture::Texture2D>&& t) {
            set_occlusion(std::move(t));
            (void)c; // prevent this from deletion
        }));
    tm->get_brdflut(
        core::job::EndCallerShared<texture::Texture2D>([this, c = std::move(c)](std::shared_ptr<texture::Texture2D>&& t) {
            set_brdflut(std::move(t));
            (void)c; // prevent this from deletion
        }));
}

gearoenix::render::material::Pbr::~Pbr() = default;

void gearoenix::render::material::Pbr::set_albedo(std::shared_ptr<texture::Texture2D>&& o)
{
    albedo = std::move(o);
}

void gearoenix::render::material::Pbr::set_normal(std::shared_ptr<texture::Texture2D>&& o)
{
    normal = std::move(o);
}

void gearoenix::render::material::Pbr::set_emission(std::shared_ptr<texture::Texture2D>&& o)
{
    emission = std::move(o);
}

void gearoenix::render::material::Pbr::set_metallic_roughness(std::shared_ptr<texture::Texture2D>&& o)
{
    metallic_roughness = std::move(o);
}

void gearoenix::render::material::Pbr::set_occlusion(std::shared_ptr<texture::Texture2D>&& o)
{
    occlusion = std::move(o);
}

void gearoenix::render::material::Pbr::set_brdflut(std::shared_ptr<texture::Texture2D>&& o)
{
    brdflut = std::move(o);
}
