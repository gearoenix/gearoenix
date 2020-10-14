#ifndef GEAROENIX_VULKAN_MEMORY_PLACE_HPP
#define GEAROENIX_VULKAN_MEMORY_PLACE_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/gx-cr-types.hpp"

namespace gearoenix::vulkan::memory {
enum struct Place : core::TypeId {
    Gpu = 1,
    Cpu = 2,
};
}
#endif
#endif