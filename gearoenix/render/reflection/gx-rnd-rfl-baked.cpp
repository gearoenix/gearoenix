#include "gx-rnd-rfl-baked.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-texture-cube.hpp"

gearoenix::render::reflection::Baked::Baked(
    engine::Engine& e,
    const std::type_index final_component_type_index,
    std::shared_ptr<texture::TextureCube>&& irradiance,
    std::shared_ptr<texture::TextureCube>&& radiance,
    const math::Aabb3<double>& include_box,
    std::string&& name)
    : Probe(e, final_component_type_index, std::move(irradiance), std::move(radiance), include_box, std::move(name))
{
}

gearoenix::render::reflection::Baked::~Baked() = default;