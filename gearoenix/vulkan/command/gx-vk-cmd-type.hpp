#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include <cstdint>

namespace gearoenix::vulkan::command {
enum struct Type : std::uint8_t {
    Primary = 1,
    Secondary = 2,
};
}
#endif