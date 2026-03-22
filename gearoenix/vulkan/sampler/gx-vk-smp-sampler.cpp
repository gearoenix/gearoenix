#include "gx-vk-smp-sampler.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../../platform/gx-plt-log.hpp"
#include "../descriptor/gx-vk-des-bindless.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"

gearoenix::vulkan::sampler::Sampler::Sampler(const render::texture::SamplerInfo& sampler_info)
    : vulkan_data(nullptr)
{
    const auto max_anisotropy = device::Physical::get().get_properties().limits.maxSamplerAnisotropy;
    const auto anisotropic_level = static_cast<float>(sampler_info.get_anisotropic_level());
    GX_ASSERT_D(anisotropic_level <= max_anisotropy);

    vk::SamplerCreateInfo info;
    info.magFilter = convert(sampler_info.get_mag_filter());
    info.minFilter = convert(sampler_info.get_min_filter());
    info.addressModeU = convert(sampler_info.get_wrap_s());
    info.addressModeV = convert(sampler_info.get_wrap_t());
    info.addressModeW = convert(sampler_info.get_wrap_r());
    info.anisotropyEnable = anisotropic_level >= 2.0f;
    info.maxAnisotropy = anisotropic_level > max_anisotropy ? max_anisotropy : anisotropic_level;
    info.borderColor = vk::BorderColor::eFloatTransparentBlack;
    info.unnormalizedCoordinates = false;
    info.compareEnable = false;
    info.compareOp = vk::CompareOp::eAlways;
    info.mipmapMode = sampler_info.needs_mipmap() ? vk::SamplerMipmapMode::eLinear : vk::SamplerMipmapMode::eNearest;
    info.mipLodBias = 0.0f;
    info.minLod = 0.0f;
    info.maxLod = sampler_info.needs_mipmap() ? vk::LodClampNone : 0.0f;
    vulkan_data = vk::raii::Sampler(device::Logical::get().get_device(), info);

    bindless_index = descriptor::Bindless::get().allocate_sampler(*vulkan_data);
}

gearoenix::vulkan::sampler::Sampler::~Sampler()
{
    descriptor::Bindless::get().free_sampler(bindless_index);
}

vk::Filter gearoenix::vulkan::sampler::Sampler::convert(const render::texture::Filter filter)
{
    switch (filter) {
    case render::texture::Filter::Linear:
    case render::texture::Filter::LinearMipmapLinear:
    case render::texture::Filter::LinearMipmapNearest:
        return vk::Filter::eLinear;
    case render::texture::Filter::Nearest:
    case render::texture::Filter::NearestMipmapLinear:
    case render::texture::Filter::NearestMipmapNearest:
        return vk::Filter::eNearest;
    default:
        GX_UNIMPLEMENTED;
    }
}

vk::SamplerAddressMode gearoenix::vulkan::sampler::Sampler::convert(const render::texture::Wrap wrap)
{
    switch (wrap) {
    case render::texture::Wrap::Repeat:
        return vk::SamplerAddressMode::eRepeat;
    case render::texture::Wrap::ClampToEdge:
        return vk::SamplerAddressMode::eClampToEdge;
    case render::texture::Wrap::Mirror:
        return vk::SamplerAddressMode::eMirroredRepeat;
    }
    GX_UNEXPECTED;
}

#endif
