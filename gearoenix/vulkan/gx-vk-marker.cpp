#include "gx-vk-marker.hpp"
#if GX_USE_DEBUG_EXTENSIONS
#include "../core/macro/gx-cr-mcr-zeroer.hpp"
#include "device/gx-vk-dev-logical.hpp"

gearoenix::vulkan::PushDebugGroup::PushDebugGroup(VkCommandBuffer cmd, float red, float green, float blue, const char* name)
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

void gearoenix::vulkan::mark(const std::string& n, const std::uint64_t o, const VkObjectType t)
{
    const auto& dev = device::Logical::get();

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
#endif