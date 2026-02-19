#include "gx-rnd-rfl-probe.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-texture-cube.hpp"

gearoenix::render::reflection::Probe::Probe(core::ecs::Entity* const entity, const core::object_type_index_t final_component_type_index, std::shared_ptr<texture::TextureCube>&& irradiance, std::shared_ptr<texture::TextureCube>&& radiance,
    const math::Aabb3<double>& include_box, std::string&& name)
    : Component(entity, final_component_type_index, std::move(name))
    , radiance_mips_count(static_cast<std::uint64_t>(RuntimeConfiguration::compute_radiance_mipmaps_count(static_cast<std::uint16_t>(radiance->get_info().get_width()))))
    , include_box(include_box)
    , irradiance(std::move(irradiance))
    , radiance(std::move(radiance))
{
}

gearoenix::render::reflection::Probe::Probe(core::ecs::Entity* const entity, const core::object_type_index_t final_component_type_index, const math::Aabb3<double>& include_box, std::string&& name)
    : Component(entity, final_component_type_index, std::move(name))
    , include_box(include_box)
{
}

gearoenix::render::reflection::Probe::~Probe() = default;

void gearoenix::render::reflection::Probe::set_radiance(std::shared_ptr<texture::TextureCube>&& t)
{
    radiance = std::move(t);
    radiance_mips_count = static_cast<std::uint64_t>(RuntimeConfiguration::compute_radiance_mipmaps_count(static_cast<std::uint16_t>(radiance->get_info().get_width())));
}
