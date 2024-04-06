#include "gx-rnd-rfl-probe.hpp"
#include "../../core/allocator/gx-cr-alc-shared-array.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-texture-cube.hpp"

gearoenix::render::reflection::Probe::Probe(
    engine::Engine& e,
    const std::type_index final_component_type_index,
    std::shared_ptr<texture::TextureCube>&& irradiance,
    std::shared_ptr<texture::TextureCube>&& radiance,
    const math::Aabb3<double>& include_box,
    std::string&& name)
    : Component(final_component_type_index, std::move(name))
    , radiance_mips_count(static_cast<std::size_t>(RuntimeConfiguration::compute_radiance_mipmaps_count(static_cast<std::uint16_t>(radiance->get_info().get_width()))))
    , include_box(include_box)
    , irradiance(std::move(irradiance))
    , radiance(std::move(radiance))
    , e(e)
{
}

gearoenix::render::reflection::Probe::~Probe() = default;

void gearoenix::render::reflection::Probe::set_scene_id(const core::ecs::entity_id_t id)
{
    scene_id = id;
}
