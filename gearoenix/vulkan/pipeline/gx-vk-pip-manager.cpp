#include "gx-vk-pip-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../device/gx-vk-dev-logical.hpp"

gearoenix::vulkan::pipeline::Manager::Manager(const device::Logical& logical_device) noexcept
    : cache(logical_device)
{
}

gearoenix::vulkan::pipeline::Manager::~Manager() noexcept = default;

#endif
