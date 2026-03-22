#include "gx-vk-pip-cache.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../device/gx-vk-dev-logical.hpp"

gearoenix::vulkan::pipeline::Cache::Cache()
    : vulkan_data(device::Logical::get().get_device(), vk::PipelineCacheCreateInfo { })
{
}

gearoenix::vulkan::pipeline::Cache::~Cache() = default;
#endif
