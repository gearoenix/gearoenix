#ifndef GEAROENIX_VULKAN_PIPELINE_CACHE_HPP
#define GEAROENIX_VULKAN_PIPELINE_CACHE_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan::device {
struct Logical;
}

namespace gearoenix::vulkan::pipeline {
struct Cache final {
    GX_GET_CRRF_PRV(device::Logical, logical_device);
    GX_GET_VAL_PRV(VkPipelineCache, vulkan_data, nullptr);

public:
    Cache(const Cache&) = delete;
    explicit Cache(const device::Logical& logical_device) noexcept;
    ~Cache() noexcept;
};
}
#endif
#endif