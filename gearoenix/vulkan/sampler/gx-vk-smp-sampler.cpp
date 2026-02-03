#include "gx-vk-smp-sampler.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../../platform/gx-plt-log.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../descriptor/gx-vk-des-bindless.hpp"
#include "../gx-vk-check.hpp"

gearoenix::vulkan::sampler::Sampler::Sampler(const render::texture::SamplerInfo& sampler_info)
{
    const auto max_anisotropy = device::Physical::get().get_properties().limits.maxSamplerAnisotropy;
    const auto anisotropic_level = static_cast<float>(sampler_info.get_anisotropic_level());
    GX_ASSERT_D(anisotropic_level <= max_anisotropy);

    VkSamplerCreateInfo info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    info.magFilter = convert(sampler_info.get_mag_filter());
    info.minFilter = convert(sampler_info.get_min_filter());
    info.addressModeU = convert(sampler_info.get_wrap_s());
    info.addressModeV = convert(sampler_info.get_wrap_t());
    info.addressModeW = convert(sampler_info.get_wrap_r());
    info.anisotropyEnable = anisotropic_level < 2.0f ? VK_FALSE : VK_TRUE;
    info.maxAnisotropy = anisotropic_level > max_anisotropy ? max_anisotropy : anisotropic_level;
    info.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    info.unnormalizedCoordinates = VK_FALSE;
    info.compareEnable = VK_FALSE;
    info.compareOp = VK_COMPARE_OP_ALWAYS;
    info.mipmapMode = sampler_info.needs_mipmap() ? VK_SAMPLER_MIPMAP_MODE_LINEAR : VK_SAMPLER_MIPMAP_MODE_NEAREST;
    info.mipLodBias = 0.0f;
    info.minLod = 0.0f;
    info.maxLod = sampler_info.needs_mipmap() ? VK_LOD_CLAMP_NONE : 0.0f;
    GX_VK_CHK(vkCreateSampler(device::Logical::get().get_vulkan_data(), &info, nullptr, &vulkan_data));

    bindless_index = descriptor::Bindless::get().allocate_sampler(vulkan_data);
}

gearoenix::vulkan::sampler::Sampler::~Sampler()
{
    descriptor::Bindless::get().free_sampler(bindless_index);
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