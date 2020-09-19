#ifndef GEAROENIX_VULKAN_MEMORY_SUB_MEMORY_HPP
#define GEAROENIX_VULKAN_MEMORY_SUB_MEMORY_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#ifdef GX_VMA_IMPL
#define VMA_IMPLEMENTATION
#define VMA_USE_STL_CONTAINERS 1
#endif
#include <vk_mem_alloc.h>
#endif
#endif