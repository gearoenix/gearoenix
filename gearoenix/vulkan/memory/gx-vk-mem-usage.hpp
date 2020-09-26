#ifndef GEAROENIX_VULKAN_MEMORY_USAGE_HPP
#define GEAROENIX_VULKAN_MEMORY_USAGE_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/gx-cr-types.hpp"

namespace gearoenix::vulkan::memory {
enum struct Usage : core::TypeId {
    Unknown = 0,
    Gpu = 1,
    Cpu = 2,
    CpuToGpu = 3,
    GpuToCpu = 4,
    CpuCopy = 5,
};
}
#endif
#endif