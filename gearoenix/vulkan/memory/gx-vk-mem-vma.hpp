#ifndef GEAROENIX_VULKAN_MEMORY_SUB_MEMORY_HPP
#define GEAROENIX_VULKAN_MEMORY_SUB_MEMORY_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN

#ifdef GX_VMA_IMPL
#define VMA_IMPLEMENTATION
#endif

#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0

#include "../../core/gx-cr-disable-warnings.hpp"
// There are several warnings in this project
#include <vk_mem_alloc.h>
// There are several warnings in this project
#include "../../core/gx-cr-enable-warnings.hpp"

#endif
#endif