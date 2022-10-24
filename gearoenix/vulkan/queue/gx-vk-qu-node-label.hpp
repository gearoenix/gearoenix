#ifndef GEAROENIX_VULKAN_QUEUE_NODE_LABEL_HPP
#define GEAROENIX_VULKAN_QUEUE_NODE_LABEL_HPP
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
#endif