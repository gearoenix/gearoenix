#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED

#include <cstdint>

namespace gearoenix::vulkan {

constexpr std::uint32_t frames_in_flight = 2;

}
#endif