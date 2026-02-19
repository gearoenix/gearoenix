#include "gx-vk-rfl-probe.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../texture/gx-vk-txt-cube.hpp"

void gearoenix::vulkan::reflection::Probe::set_textures(const texture::TextureCube& irr, const texture::TextureCube& rad)
{
    irradiance_texture_index = irr.get_view_index();
    irradiance_sampler_index = irr.get_sampler_index();
    radiance_texture_index = rad.get_view_index();
    radiance_sampler_index = rad.get_sampler_index();
}

gearoenix::vulkan::reflection::Probe::~Probe() = default;

#endif