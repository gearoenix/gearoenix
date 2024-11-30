#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED

namespace gearoenix::vulkan::queue {
enum struct NodeLabel : unsigned int {
    Start,
    ImGUI,
    End,
};
}
#endif