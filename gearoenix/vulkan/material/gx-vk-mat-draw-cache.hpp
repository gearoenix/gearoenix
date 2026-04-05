#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan::material {
struct DrawCache final {
    std::uint32_t material_index = ~0;
    std::uint8_t forward_pipeline_index = ~0;
    std::uint8_t shadow_pipeline_index = ~0;
};
}

#endif