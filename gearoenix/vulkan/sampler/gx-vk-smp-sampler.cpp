#include "gx-vk-smp-sampler.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../../platform/gx-plt-log.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../gx-vk-check.hpp"

gearoenix::vulkan::sampler::Sampler::Sampler(const render::texture::SamplerInfo& sampler_info)
{
    VkSamplerCreateInfo info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    info.magFilter = convert(sampler_info.get_mag_filter());
    info.minFilter = convert(sampler_info.get_min_filter());
    info.addressModeU = convert(sampler_info.get_wrap_r());
    info.addressModeV = convert(sampler_info.get_wrap_s());
    info.addressModeW = convert(sampler_info.get_wrap_t());
    // TODO check for anisotropy support in device
    info.anisotropyEnable = sampler_info.get_anisotropic_level() == 0 ? VK_FALSE : VK_TRUE;
    info.maxAnisotropy = sampler_info.get_anisotropic_level();
    info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    info.unnormalizedCoordinates = VK_FALSE;
    info.compareEnable = VK_FALSE;
    info.compareOp = VK_COMPARE_OP_ALWAYS;
    info.mipmapMode = sampler_info.needs_mipmap() ? VK_SAMPLER_MIPMAP_MODE_LINEAR : VK_SAMPLER_MIPMAP_MODE_NEAREST;
    info.mipLodBias = 0.0f;
    info.minLod = 0.0f;
    info.maxLod = 0.0f;
    GX_VK_CHK(vkCreateSampler(device::Logical::get().get_vulkan_data(), &info, nullptr, &vulkan_data));
}

gearoenix::vulkan::sampler::Sampler::~Sampler()
{
    vkDestroySampler(device::Logical::get().get_vulkan_data(), vulkan_data, nullptr);
}

VkFilter gearoenix::vulkan::sampler::Sampler::convert(const render::texture::Filter filter)
{
    switch (filter) {
    case render::texture::Filter::Linear:
    case render::texture::Filter::LinearMipmapLinear:
    case render::texture::Filter::LinearMipmapNearest:
        return VK_FILTER_LINEAR;
    case render::texture::Filter::Nearest:
    case render::texture::Filter::NearestMipmapLinear:
    case render::texture::Filter::NearestMipmapNearest:
        return VK_FILTER_NEAREST;
    default:
        GX_UNIMPLEMENTED;
    }
}

VkSamplerAddressMode gearoenix::vulkan::sampler::Sampler::convert(const render::texture::Wrap wrap)
{
    switch (wrap) {
    case render::texture::Wrap::Repeat:
        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    case render::texture::Wrap::ClampToEdge:
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    case render::texture::Wrap::Mirror:
        return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    }
    GX_UNEXPECTED;
}

#endif