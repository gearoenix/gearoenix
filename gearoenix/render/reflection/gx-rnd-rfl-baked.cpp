#include "gx-rnd-rfl-baked.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-texture-cube.hpp"

gearoenix::render::reflection::Baked::Baked(
    engine::Engine& e,
    std::shared_ptr<texture::TextureCube> irradiance,
    std::shared_ptr<texture::TextureCube> radiance,
    math::Aabb3<double> include_box,
    std::string&& name) noexcept
    : core::ecs::Component(this, std::move(name))
    , radiance_mips_count(static_cast<std::size_t>(RuntimeConfiguration::compute_radiance_mipmaps_count(static_cast<std::uint16_t>(radiance->get_info().width))))
    , include_box(include_box)
    , irradiance(std::move(irradiance))
    , radiance(std::move(radiance))
    , e(e)
{
}

gearoenix::render::reflection::Baked::~Baked() noexcept = default;

gearoenix::render::reflection::Baked::Baked(Baked&&) noexcept = default;