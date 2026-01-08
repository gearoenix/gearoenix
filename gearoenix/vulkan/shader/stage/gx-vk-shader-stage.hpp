#pragma once
#include "../../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../gx-vk-loader.hpp"

#include <cstdint>
#include <span>

namespace gearoenix::vulkan::shader::stage {
struct Stage {
    GX_GET_CVAL_PRT(VkShaderModule, vulkan_data);

public:
    explicit Stage(std::span<std::uint8_t> data);
    virtual ~Stage();
};
}
#endif