#include "gx-rnd-rfl-baked.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-texture-cube.hpp"

gearoenix::render::reflection::Baked::Baked(
    engine::Engine& e,
    std::shared_ptr<texture::TextureCube> irradiance,
    std::shared_ptr<texture::TextureCube> radiance,
    math::Aabb3<double> include_box) noexcept
    : core::ecs::Component(this)
    , include_box(include_box)
    , irradiance(std::move(irradiance))
    , radiance(std::move(radiance))
    , e(e)
{
}

gearoenix::render::reflection::Baked::~Baked() noexcept = default;

gearoenix::render::reflection::Baked::Baked(Baked&&) noexcept = default;