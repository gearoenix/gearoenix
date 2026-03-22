#include "gx-vk-marker.hpp"
#if GX_USE_DEBUG_EXTENSIONS
#include "device/gx-vk-dev-logical.hpp"

void gearoenix::vulkan::mark_impl(const std::string& name, const std::uint64_t handle, const vk::ObjectType type)
{
    const auto& dev = device::Logical::get();

    if (!dev.get_debug_marker_is_available()) {
        return;
    }

    vk::DebugUtilsObjectNameInfoEXT info;
    info.objectType = type;
    info.objectHandle = handle;
    info.pObjectName = name.c_str();
    dev.get_vulkan_data().setDebugUtilsObjectNameEXT(info);
}

gearoenix::vulkan::PushDebugGroup::PushDebugGroup(const vk::CommandBuffer cmd, const float red, const float green, const float blue, const char* const name)
    : cmd(cmd)
{
    vk::DebugUtilsLabelEXT info;
    info.pLabelName = name;
    info.color = { { red, green, blue, 1.0f } };
    cmd.beginDebugUtilsLabelEXT(info);
}

gearoenix::vulkan::PushDebugGroup::~PushDebugGroup() { cmd.endDebugUtilsLabelEXT(); }

#endif
