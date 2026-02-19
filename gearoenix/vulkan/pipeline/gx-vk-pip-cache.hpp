#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan::pipeline {
struct Cache final {
    GX_GET_VAL_PRV(VkPipelineCache, vulkan_data, nullptr);

public:
    Cache();
    Cache(Cache&&) = delete;
    Cache(const Cache&) = delete;
    ~Cache();
};
}
#endif