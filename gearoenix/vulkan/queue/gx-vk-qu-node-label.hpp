#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include <cstdint>

namespace gearoenix::vulkan::queue {
enum struct NodeLabel : std::uint8_t {
    Start,
    ImGUI,
    End,
};
}
#endif