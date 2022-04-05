#include "gx-rnd-mat-pbr.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"

gearoenix::render::material::Pbr::Pbr(engine::Engine& e, const core::sync::EndCallerIgnored& c) noexcept
    : albedo_factor(1.0f)
    , emission_roughness_factor(1.0f)
    , normal_metallic_factor(1.0f)
    , alpha_cutoff_occlusion_strength_radiance_lod_coefficient_reserved(0.001f, 1.0f, 0.0f, 0.0f)
{
    const auto& tm = e.get_texture_manager();
    albedo = tm->get_checker(c);
    normal = tm->create_2d_from_colour(math::Vec4(0.5f, 0.5f, 1.0f, 0.0f), c);
    emission = tm->create_2d_from_colour(math::Vec4(0.0f, 0.0f, 0.0f, 0.0f), c);
    metallic_roughness = tm->create_2d_from_colour(math::Vec4(0.001f, 0.5f, 0.0f, 0.0f), c);
    occlusion = tm->create_2d_from_colour(math::Vec4(1.0f, 1.0f, 1.0f, 1.0f), c);
}

gearoenix::render::material::Pbr::~Pbr() noexcept = default;

gearoenix::render::material::Pbr::Pbr(const Pbr&) noexcept = default;

gearoenix::render::material::Pbr::Pbr(Pbr&&) noexcept = default;

gearoenix::render::material::Pbr& gearoenix::render::material::Pbr::operator=(Pbr&&) noexcept = default;

gearoenix::render::material::Pbr& gearoenix::render::material::Pbr::operator=(const Pbr&) noexcept = default;

void gearoenix::render::material::Pbr::randomise_albedo() noexcept
{
    albedo_factor.x = static_cast<float>((std::rand() & 127) + 128) / 255.0f;
    albedo_factor.y = static_cast<float>((std::rand() & 127) + 128) / 255.0f;
    albedo_factor.z = static_cast<float>((std::rand() & 127) + 128) / 255.0f;
}
