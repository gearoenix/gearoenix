#include "gx-vk-des-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED

gearoenix::vulkan::descriptor::Manager::Manager(const device::Logical& logical_device) noexcept
    : imgui(Pool::create_imgui(logical_device))
{
}

gearoenix::vulkan::descriptor::Manager::~Manager() noexcept = default;

#endif
