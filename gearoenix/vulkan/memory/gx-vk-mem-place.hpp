#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include <cstdint>

namespace gearoenix::vulkan::memory {
enum struct Place : std::uint8_t {
    Gpu = 1,
    Cpu = 2,
};
}
#endif