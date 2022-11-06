#include "gx-rnd-mat-pbr.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"

gearoenix::render::material::Pbr::Pbr(engine::Engine& e, const core::sync::EndCallerIgnored& c) noexcept
    : Material(Id::Pbr)
    , albedo_factor(1.0f)
    , emission_roughness_factor(1.0f)
    , normal_metallic_factor(1.0f)
    , alpha_cutoff_occlusion_strength_reserved_reserved(0.001f, 1.0f, 0.0f, 0.0f)
{
    const auto& tm = e.get_texture_manager();
    albedo = tm->create_2d_from_colour(math::Vec4(1.0f, 1.0f, 1.0f, 1.0f), c);
    normal = tm->create_2d_from_colour(math::Vec4(0.5f, 0.5f, 1.0f, 0.0f), c);
    emission = tm->create_2d_from_colour(math::Vec4(0.0f, 0.0f, 0.0f, 0.0f), c);
    metallic_roughness = tm->create_2d_from_colour(math::Vec4(1.0f, 1.0f, 0.0f, 0.0f), c);
    occlusion = tm->create_2d_from_colour(math::Vec4(1.0f, 1.0f, 1.0f, 1.0f), c);
    brdflut = tm->get_brdflut(c);
}

gearoenix::render::material::Pbr::~Pbr() noexcept = default;

void gearoenix::render::material::Pbr::set_albedo(std::shared_ptr<texture::Texture2D>&& o) noexcept
{
    albedo = std::move(o);
}

void gearoenix::render::material::Pbr::set_normal(std::shared_ptr<texture::Texture2D>&& o) noexcept
{
    normal = std::move(o);
}

void gearoenix::render::material::Pbr::set_emission(std::shared_ptr<texture::Texture2D>&& o) noexcept
{
    emission = std::move(o);
}

void gearoenix::render::material::Pbr::set_metallic_roughness(std::shared_ptr<texture::Texture2D>&& o) noexcept
{
    metallic_roughness = std::move(o);
}

void gearoenix::render::material::Pbr::set_occlusion(std::shared_ptr<texture::Texture2D>&& o) noexcept
{
    occlusion = std::move(o);
}
