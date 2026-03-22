#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "gx-vk-loader.hpp"
#if GX_USE_DEBUG_EXTENSIONS
#include "../core/macro/gx-cr-mcr-concatenate.hpp"

#include <sstream>
#include <string>

namespace gearoenix::vulkan::device {
struct Logical;
}

namespace gearoenix::vulkan {

void mark_impl(const std::string& name, std::uint64_t handle, vk::ObjectType type);

template <typename T>
void mark(const std::string& name, T handle)
{
    mark_impl(name, reinterpret_cast<std::uint64_t>(static_cast<typename T::NativeType>(handle)), T::objectType);
}

struct PushDebugGroup final {
    const vk::CommandBuffer cmd;

    PushDebugGroup(vk::CommandBuffer cmd, float red, float green, float blue, const char* name);
    ~PushDebugGroup();
};

}

#define GX_VK_MARK(name, obj) gearoenix::vulkan::mark(name, obj)

#define GX_VK_PUSH_DEBUG_GROUP_STR_VAR GX_CONCAT(_gearoenix_vulkan_push_debug_str_, __LINE__)
#define GX_VK_PUSH_DEBUG_GROUP_OBJ_VAR GX_CONCAT(_gearoenix_vulkan_push_debug_obj_, __LINE__)

#define GX_VK_PUSH_DEBUG_GROUP(cmd, red, green, blue, ...)                           \
    thread_local std::string GX_VK_PUSH_DEBUG_GROUP_STR_VAR;                         \
    GX_VK_PUSH_DEBUG_GROUP_STR_VAR.clear();                                          \
    std::format_to(std::back_inserter(GX_VK_PUSH_DEBUG_GROUP_STR_VAR), __VA_ARGS__); \
    const gearoenix::vulkan::PushDebugGroup GX_VK_PUSH_DEBUG_GROUP_OBJ_VAR(cmd, red, green, blue, GX_VK_PUSH_DEBUG_GROUP_STR_VAR.c_str());

#else
#define GX_VK_MARK(name, obj) static_assert(true, "")
#define GX_VK_PUSH_DEBUG_GROUP(cmd, red, green, blue, ...) static_assert(true, "")
#endif
#endif
