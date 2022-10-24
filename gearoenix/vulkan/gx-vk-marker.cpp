#include "gx-vk-marker.hpp"
#ifdef GX_USE_DEBUG_EXTENSIONS
#include "../core/macro/gx-cr-mcr-zeroer.hpp"
#include "device/gx-vk-dev-logical.hpp"

void gearoenix::vulkan::mark(
    const std::string& n,
    const std::uint64_t o,
    const VkDebugReportObjectTypeEXT t,
    const device::Logical& d) noexcept
{
    if (nullptr == vkDebugMarkerSetObjectNameEXT)
        return;
    if (!d.get_debug_marker_is_available())
        return;
    VkDebugMarkerObjectNameInfoEXT info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT;
    info.objectType = t;
    info.object = o;
    info.pObjectName = n.c_str();
    vkDebugMarkerSetObjectNameEXT(d.get_vulkan_data(), &info);
}
#endif