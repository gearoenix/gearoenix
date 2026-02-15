#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED

#include <cstdint>

namespace gearoenix::vulkan {

/// Maximum number of frames that can be processed concurrently by the GPU.
/// Currently limited to 2 by the buffer manager and descriptor manager designs.
/// Empirically, higher values showed no FPS improvement.
constexpr std::uint32_t frames_in_flight = 2;

}
#endif