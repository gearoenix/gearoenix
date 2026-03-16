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

struct PushDebugGroup final {
    const VkCommandBuffer cmd;

    PushDebugGroup(VkCommandBuffer cmd, float red, float green, float blue, const char* name);
    ~PushDebugGroup();
};

void mark(const std::string& name, VkAccelerationStructureKHR o);
void mark(const std::string& name, VkBuffer o);
void mark(const std::string& name, VkBufferView o);
void mark(const std::string& name, VkCommandBuffer o);
void mark(const std::string& name, VkCommandPool o);
void mark(const std::string& name, VkDescriptorPool o);
void mark(const std::string& name, VkDescriptorSet o);
void mark(const std::string& name, VkDescriptorSetLayout o);
void mark(const std::string& name, VkDevice o);
void mark(const std::string& name, VkDeviceMemory o);
void mark(const std::string& name, VkEvent o);
void mark(const std::string& name, VkFence o);
void mark(const std::string& name, VkFramebuffer o);
void mark(const std::string& name, VkImage o);
void mark(const std::string& name, VkImageView o);
void mark(const std::string& name, VkInstance o);
void mark(const std::string& name, VkPhysicalDevice o);
void mark(const std::string& name, VkPipeline o);
void mark(const std::string& name, VkPipelineCache o);
void mark(const std::string& name, VkPipelineLayout o);
void mark(const std::string& name, VkQueryPool o);
void mark(const std::string& name, VkQueue o);
void mark(const std::string& name, VkRenderPass o);
void mark(const std::string& name, VkSampler o);
void mark(const std::string& name, VkSemaphore o);
void mark(const std::string& name, VkShaderModule o);
void mark(const std::string& name, VkSurfaceKHR o);
void mark(const std::string& name, VkSwapchainKHR o);
}

#define GX_VK_MARK(name, obj) mark(name, obj)

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