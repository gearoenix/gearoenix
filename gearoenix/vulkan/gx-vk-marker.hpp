#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "gx-vk-loader.hpp"
#if GX_USE_DEBUG_EXTENSIONS
#include <cstdint>
#include <string>
#include <type_traits>

namespace gearoenix::vulkan::device {
struct Logical;
}

namespace gearoenix::vulkan {
void mark(const std::string&, std::uint64_t, VkObjectType, const device::Logical& dev);

template <typename T>
void mark(const std::string& name, T o, const device::Logical& dev)
{
    if constexpr (std::is_same_v<T, VkAccelerationStructureKHR>) {
        mark(name, reinterpret_cast<std::uint64_t>(o), VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR, dev);
    } else if constexpr (std::is_same_v<T, VkBuffer>) {
        mark(name, reinterpret_cast<std::uint64_t>(o), VK_OBJECT_TYPE_BUFFER, dev);
    } else if constexpr (std::is_same_v<T, VkCommandBuffer>) {
        mark(name, reinterpret_cast<std::uint64_t>(o), VK_OBJECT_TYPE_COMMAND_BUFFER, dev);
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