#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "gx-vk-loader.hpp"
#ifdef GX_USE_DEBUG_EXTENSIONS
#include <cstdint>
#include <string>
#include <type_traits>

namespace gearoenix::vulkan::device {
struct Logical;
}

namespace gearoenix::vulkan {
void mark(const std::string&, std::uint64_t, VkDebugReportObjectTypeEXT, const device::Logical& dev);

template <typename T>
void mark(const std::string& name, T o, const device::Logical& dev)
{
    if constexpr (std::is_same_v<T, VkAccelerationStructureKHR>) {
        mark(name, reinterpret_cast<std::uint64_t>(o), VK_DEBUG_REPORT_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR_EXT, dev);
    } else if constexpr (std::is_same_v<T, VkBuffer>) {
        mark(name, reinterpret_cast<std::uint64_t>(o), VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT, dev);
    } else if constexpr (std::is_same_v<T, VkCommandBuffer>) {
        mark(name, reinterpret_cast<std::uint64_t>(o), VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT, dev);
    } else {
        static_assert("Unknown type for marking");
    }
}
}

#define GX_VK_MARK(name, obj, logical_device) mark(name, obj, logical_device)

#else
#define GX_VK_MARK(name, obj, logical_device)
#endif
#endif