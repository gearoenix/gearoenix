#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../render/texture/gx-rnd-txt-sampler.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan::sampler {
struct Sampler final {
    GX_GET_VAL_PRV(VkSampler, vulkan_data, nullptr);
    GX_GET_VAL_PRV(std::uint32_t, bindless_index, static_cast<std::uint32_t>(-1));

public:
    explicit Sampler(const render::texture::SamplerInfo& sampler_info);
    ~Sampler();
    [[nodiscard]] static VkFilter convert(render::texture::Filter filter);
    [[nodiscard]] static VkSamplerAddressMode convert(render::texture::Wrap wrap);
};
}
#endif