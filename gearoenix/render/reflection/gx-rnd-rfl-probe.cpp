#include "gx-rnd-rfl-probe.hpp"
#include "../buffer/gx-rnd-buf-manager.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-texture-cube.hpp"
#include "gx-rnd-rfl-uniform.hpp"

gearoenix::render::reflection::Probe::Probe(
    core::ecs::Entity* const entity,
    const core::object_type_index_t final_component_type_index,
    std::shared_ptr<texture::TextureCube>&& irradiance,
    std::shared_ptr<texture::TextureCube>&& radiance,
    const math::Aabb3<core::fp_t>& include_box,
    std::string&& name)
    : Component(entity, final_component_type_index, std::move(name))
    , radiance_mips_count(static_cast<std::uint64_t>(RuntimeConfiguration::compute_radiance_mipmaps_count(static_cast<std::uint16_t>(radiance->get_info().get_width()))))
    , include_box(include_box)
    , irradiance(std::move(irradiance))
    , radiance(std::move(radiance))
    , uniform(buffer::Manager::get_range(buffer::UniformRegionIndex::reflection_probes))
{
}

gearoenix::render::reflection::Probe::Probe(
    core::ecs::Entity* const entity,
    const core::object_type_index_t final_component_type_index,
    const math::Aabb3<core::fp_t>& include_box,
    std::string&& name)
    : Component(entity, final_component_type_index, std::move(name))
    , include_box(include_box)
    , uniform(buffer::Manager::get_range(buffer::UniformRegionIndex::reflection_probes))
{
}

gearoenix::render::reflection::Probe::~Probe() = default;

void gearoenix::render::reflection::Probe::set_radiance(std::shared_ptr<texture::TextureCube>&& t)
{
    radiance = std::move(t);
    radiance_mips_count = static_cast<std::uint64_t>(RuntimeConfiguration::compute_radiance_mipmaps_count(static_cast<std::uint16_t>(radiance->get_info().get_width())));
}

void gearoenix::render::reflection::Probe::update_uniform()
{
    auto& ref = uniform.get_ref<GxShaderDataReflectionProbe>();
    ref.position_radius = {
        include_box.get_center().to<float>(),
        static_cast<float>(include_box.get_diameter().length()) * 0.5f
    };
    ref.irradiance_radiance_texture_sampler_index = {
        irradiance->get_shader_resource_index(),
        irradiance->get_sampler_shader_resource_index(),
        radiance->get_shader_resource_index(),
        radiance->get_sampler_shader_resource_index()
    };
    ref.radiance_lod_coefficient.x = static_cast<float>(radiance_mips_count - 1);
}
