#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../render/texture/gx-rnd-txt-sampler.hpp"
#include "../gx-vk-loader.hpp"

#include <memory>

namespace gearoenix::vulkan::device {
struct Logical;
}

namespace gearoenix::vulkan::sampler {
struct Sampler final {
    GX_GET_REFC_PRV(std::shared_ptr<device::Logical>, logical_device);
    GX_GET_VAL_PRV(VkSampler, vulkan_data, nullptr);

public:
    explicit Sampler(
        std::shared_ptr<device::Logical> logical_device,
        const render::texture::SamplerInfo& sampler_info);
    ~Sampler();
    [[nodiscard]] static VkFilter convert(render::texture::Filter filter);
    [[nodiscard]] static VkSamplerAddressMode convert(render::texture::Wrap wrap);
};
}
#endif