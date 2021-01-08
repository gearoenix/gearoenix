#ifndef GEAROENIX_VULKAN_MEMORY_PLACE_HPP
#define GEAROENIX_VULKAN_MEMORY_PLACE_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-types.hpp"

namespace gearoenix::vulkan::memory {
enum struct Place : core::TypeId {
    Gpu = 1,
    Cpu = 2,
};
}

#endif

#endif