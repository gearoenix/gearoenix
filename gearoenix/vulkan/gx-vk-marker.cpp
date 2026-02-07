#include "gx-vk-marker.hpp"
#if GX_USE_DEBUG_EXTENSIONS
#include "../core/macro/gx-cr-mcr-zeroer.hpp"
#include "device/gx-vk-dev-logical.hpp"

namespace {
void mark(const std::string& n, const std::uint64_t o, const VkObjectType t)
{
    const auto& dev = gearoenix::vulkan::device::Logical::get();

    if (nullptr == vkSetDebugUtilsObjectNameEXT) {
        return;
    }

    if (!dev.get_debug_marker_is_available()) {
        return;
    }

    VkDebugUtilsObjectNameInfoEXT info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
    info.objectType = t;
    info.objectHandle = o;
    info.pObjectName = n.c_str();
    vkSetDebugUtilsObjectNameEXT(dev.get_vulkan_data(), &info);
}
}

gearoenix::vulkan::PushDebugGroup::PushDebugGroup(const VkCommandBuffer cmd, const float red, const float green, const float blue, const char* const name)
    : cmd(cmd)
{
    const VkDebugUtilsLabelEXT info {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT,
        .pNext = nullptr,
        .pLabelName = name,
        .color = { red, green, blue, 1.0f },
    };
    vkCmdBeginDebugUtilsLabelEXT(cmd, &info);
}

gearoenix::vulkan::PushDebugGroup::~PushDebugGroup()
{
    vkCmdEndDebugUtilsLabelEXT(cmd);
}

void gearoenix::vulkan::mark(const std::string& name, const VkAccelerationStructureKHR o)
{
    ::mark(name, reinterpret_cast<std::uint64_t>(o), VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR);
}

void gearoenix::vulkan::mark(const std::string& name, const VkBuffer o)
{
    ::mark(name, reinterpret_cast<std::uint64_t>(o), VK_OBJECT_TYPE_BUFFER);
}

void gearoenix::vulkan::mark(const std::string& name, const VkBufferView o)
{
    ::mark(name, reinterpret_cast<std::uint64_t>(o), VK_OBJECT_TYPE_BUFFER_VIEW);
}

void gearoenix::vulkan::mark(const std::string& name, const VkCommandBuffer o)
{
    ::mark(name, reinterpret_cast<std::uint64_t>(o), VK_OBJECT_TYPE_COMMAND_BUFFER);
}

void gearoenix::vulkan::mark(const std::string& name, const VkCommandPool o)
{
    ::mark(name, reinterpret_cast<std::uint64_t>(o), VK_OBJECT_TYPE_COMMAND_POOL);
}

void gearoenix::vulkan::mark(const std::string& name, const VkDescriptorPool o)
{
    ::mark(name, reinterpret_cast<std::uint64_t>(o), VK_OBJECT_TYPE_DESCRIPTOR_POOL);
}

void gearoenix::vulkan::mark(const std::string& name, const VkDescriptorSet o)
{
    ::mark(name, reinterpret_cast<std::uint64_t>(o), VK_OBJECT_TYPE_DESCRIPTOR_SET);
}

void gearoenix::vulkan::mark(const std::string& name, const VkDescriptorSetLayout o)
{
    ::mark(name, reinterpret_cast<std::uint64_t>(o), VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT);
}

void gearoenix::vulkan::mark(const std::string& name, const VkDevice o)
{
    ::mark(name, reinterpret_cast<std::uint64_t>(o), VK_OBJECT_TYPE_DEVICE);
}

void gearoenix::vulkan::mark(const std::string& name, const VkDeviceMemory o)
{
    ::mark(name, reinterpret_cast<std::uint64_t>(o), VK_OBJECT_TYPE_DEVICE_MEMORY);
}

void gearoenix::vulkan::mark(const std::string& name, const VkEvent o)
{
    ::mark(name, reinterpret_cast<std::uint64_t>(o), VK_OBJECT_TYPE_EVENT);
}

void gearoenix::vulkan::mark(const std::string& name, const VkFence o)
{
    ::mark(name, reinterpret_cast<std::uint64_t>(o), VK_OBJECT_TYPE_FENCE);
}

void gearoenix::vulkan::mark(const std::string& name, const VkFramebuffer o)
{
    ::mark(name, reinterpret_cast<std::uint64_t>(o), VK_OBJECT_TYPE_FRAMEBUFFER);
}

void gearoenix::vulkan::mark(const std::string& name, const VkImage o)
{
    ::mark(name, reinterpret_cast<std::uint64_t>(o), VK_OBJECT_TYPE_IMAGE);
}

void gearoenix::vulkan::mark(const std::string& name, const VkImageView o)
{
    ::mark(name, reinterpret_cast<std::uint64_t>(o), VK_OBJECT_TYPE_IMAGE_VIEW);
}

void gearoenix::vulkan::mark(const std::string& name, const VkInstance o)
{
    ::mark(name, reinterpret_cast<std::uint64_t>(o), VK_OBJECT_TYPE_INSTANCE);
}

void gearoenix::vulkan::mark(const std::string& name, const VkPhysicalDevice o)
{
    ::mark(name, reinterpret_cast<std::uint64_t>(o), VK_OBJECT_TYPE_PHYSICAL_DEVICE);
}

void gearoenix::vulkan::mark(const std::string& name, const VkPipeline o)
{
    ::mark(name, reinterpret_cast<std::uint64_t>(o), VK_OBJECT_TYPE_PIPELINE);
}

void gearoenix::vulkan::mark(const std::string& name, const VkPipelineCache o)
{
    ::mark(name, reinterpret_cast<std::uint64_t>(o), VK_OBJECT_TYPE_PIPELINE_CACHE);
}

void gearoenix::vulkan::mark(const std::string& name, const VkPipelineLayout o)
{
    ::mark(name, reinterpret_cast<std::uint64_t>(o), VK_OBJECT_TYPE_PIPELINE_LAYOUT);
}

void gearoenix::vulkan::mark(const std::string& name, const VkQueryPool o)
{
    ::mark(name, reinterpret_cast<std::uint64_t>(o), VK_OBJECT_TYPE_QUERY_POOL);
}

void gearoenix::vulkan::mark(const std::string& name, const VkQueue o)
{
    ::mark(name, reinterpret_cast<std::uint64_t>(o), VK_OBJECT_TYPE_QUEUE);
}

void gearoenix::vulkan::mark(const std::string& name, const VkRenderPass o)
{
    ::mark(name, reinterpret_cast<std::uint64_t>(o), VK_OBJECT_TYPE_RENDER_PASS);
}

void gearoenix::vulkan::mark(const std::string& name, const VkSampler o)
{
    ::mark(name, reinterpret_cast<std::uint64_t>(o), VK_OBJECT_TYPE_SAMPLER);
}

void gearoenix::vulkan::mark(const std::string& name, const VkSemaphore o)
{
    ::mark(name, reinterpret_cast<std::uint64_t>(o), VK_OBJECT_TYPE_SEMAPHORE);
}

void gearoenix::vulkan::mark(const std::string& name, const VkShaderModule o)
{
    ::mark(name, reinterpret_cast<std::uint64_t>(o), VK_OBJECT_TYPE_SHADER_MODULE);
}

void gearoenix::vulkan::mark(const std::string& name, const VkSurfaceKHR o)
{
    ::mark(name, reinterpret_cast<std::uint64_t>(o), VK_OBJECT_TYPE_SURFACE_KHR);
}

void gearoenix::vulkan::mark(const std::string& name, const VkSwapchainKHR o)
{
    ::mark(name, reinterpret_cast<std::uint64_t>(o), VK_OBJECT_TYPE_SWAPCHAIN_KHR);
}

#endif