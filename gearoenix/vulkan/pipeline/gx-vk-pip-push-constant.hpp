#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED

#include <cstdint>

namespace gearoenix::vulkan::pipeline {
struct PushConstants final {
    std::uint32_t scene_index = static_cast<std::uint32_t>(-1);
    std::uint32_t camera_index = static_cast<std::uint32_t>(-1);
    std::uint32_t model_index = static_cast<std::uint32_t>(-1);
    std::uint32_t material_index = static_cast<std::uint32_t>(-1);
    std::uint32_t camera_model_index = static_cast<std::uint32_t>(-1);
};
}
#endif
