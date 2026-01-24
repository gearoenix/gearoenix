#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "gx-vk-loader.hpp"
#if GX_USE_DEBUG_EXTENSIONS
#include "../core/macro/gx-cr-mcr-concatenate.hpp"

#include <cstdint>
#include <string>
#include <sstream>
#include <type_traits>
#include <format>
#include <iterator>

namespace gearoenix::vulkan::device {
struct Logical;
}

namespace gearoenix::vulkan {

struct PushDebugGroup final {
    const VkCommandBuffer cmd;

    PushDebugGroup(VkCommandBuffer cmd, float red, float green, float blue, const char* name);
    ~PushDebugGroup();
};

void mark(const std::string&, std::uint64_t, VkObjectType);

template <typename T>
void mark(const std::string& name, T o)
{
    if constexpr (std::is_same_v<T, VkAccelerationStructureKHR>) {
        mark(name, reinterpret_cast<std::uint64_t>(o), VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR);
    } else if constexpr (std::is_same_v<T, VkBuffer>) {
        mark(name, reinterpret_cast<std::uint64_t>(o), VK_OBJECT_TYPE_BUFFER);
    } else if constexpr (std::is_same_v<T, VkCommandBuffer>) {
        mark(name, reinterpret_cast<std::uint64_t>(o), VK_OBJECT_TYPE_COMMAND_BUFFER);
    } else {
        static_assert("Unknown type for marking");
    }
}
}

#define GX_VK_MARK(name, obj) mark(name, obj)

#define GX_VK_PUSH_DEBUG_GROUP_STR_VAR GX_CONCAT_5(_gearoenix_vulkan_push_debug_str_, __LINE__)
#define GX_VK_PUSH_DEBUG_GROUP_OBJ_VAR GX_CONCAT_5(_gearoenix_vulkan_push_debug_obj_, __LINE__)

#define GX_VK_PUSH_DEBUG_GROUP(cmd, red, green, blue, ...) \
    thread_local std::string GX_VK_PUSH_DEBUG_GROUP_STR_VAR; \
    GX_VK_PUSH_DEBUG_GROUP_STR_VAR.clear(); \
    std::format_to(std::back_inserter(GX_VK_PUSH_DEBUG_GROUP_STR_VAR), __VA_ARGS__); \
    const gearoenix::vulkan::PushDebugGroup GX_VK_PUSH_DEBUG_GROUP_OBJ_VAR(cmd, red, green, blue, GX_VK_PUSH_DEBUG_GROUP_STR_VAR.c_str());

#else
#define GX_VK_MARK(name, obj) static_assert(true, "")
#define GX_VK_PUSH_DEBUG_GROUP(cmd, red, green, blue, ...) static_assert(true, "")
#endif
#endif