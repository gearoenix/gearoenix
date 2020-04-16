#include "rnd-rfl-baked.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../texture/rnd-txt-texture-cube.hpp"

gearoenix::render::reflection::Baked::Baked(
    std::shared_ptr<texture::TextureCube> irr,
    std::shared_ptr<texture::TextureCube> rad) noexcept
    : Reflection(core::asset::Manager::create_id(), Type::Baked, irr->get_render_engine())
{
    irradiance = std::move(irr);
    radiance = std::move(rad);
}

gearoenix::render::reflection::Baked::~Baked() noexcept = default;
