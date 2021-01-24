#ifndef GEAROENIX_VULKAN_COMMAND_TYPE_HPP
#define GEAROENIX_VULKAN_COMMAND_TYPE_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-types.hpp"

namespace gearoenix::vulkan::command {
enum struct Type : core::TypeId {
    Primary = 1,
    Secondary = 2,
};
}

#endif
#endif