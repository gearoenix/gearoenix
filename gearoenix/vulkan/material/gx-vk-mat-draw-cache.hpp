#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED

#include <cstdint>

namespace gearoenix::vulkan::material {
struct DrawCache final {
    std::uint32_t material_index = static_cast<std::uint32_t>(-1);
    std::uint8_t forward_pipeline_index = static_cast<std::uint8_t>(-1);
    std::uint8_t shadow_pipeline_index = static_cast<std::uint8_t>(-1);
};
}

#endif