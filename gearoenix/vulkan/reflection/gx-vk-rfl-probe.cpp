#include "gx-vk-rfl-probe.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../texture/gx-vk-txt-cube.hpp"
#include "gx-vk-rfl-manager.hpp"

void gearoenix::vulkan::reflection::Probe::set_textures(const texture::TextureCube& irr, const texture::TextureCube& rad)
{
    irradiance_texture_index = irr.get_view_index();
    irradiance_sampler_index = irr.get_sampler_index();
    radiance_texture_index = rad.get_view_index();
    radiance_sampler_index = rad.get_sampler_index();
}

gearoenix::vulkan::reflection::Probe::~Probe() = default;

void gearoenix::vulkan::reflection::Probe::after_record(const std::uint64_t frame_number)
{
    if (frame_number == last_frame_number_update) {
        return;
    }
    last_frame_number_update = frame_number;

    auto sd = uniform_indexer_t::get().get_next();
    auto& ref = *sd.get_ptr();

    ref.irradiance_texture_index = irradiance_texture_index;
    ref.irradiance_sampler_index = irradiance_sampler_index;

    ref.radiance_texture_index = radiance_texture_index;
    ref.radiance_sampler_index = radiance_sampler_index;

    ref.radiance_lod_coefficient = radiance_lod_coefficient;

    shader_data_index = sd.get_index();
}

#endif