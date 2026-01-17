#include "gx-vk-pip-cache.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../gx-vk-check.hpp"

gearoenix::vulkan::pipeline::Cache::Cache()
{
    VkPipelineCacheCreateInfo info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    GX_VK_CHK(vkCreatePipelineCache(device::Logical::get().get_vulkan_data(), &info, nullptr, &vulkan_data));
}

gearoenix::vulkan::pipeline::Cache::~Cache()
{
    if (nullptr != vulkan_data) {
        vkDestroyPipelineCache(device::Logical::get().get_vulkan_data(), vulkan_data, nullptr);
        vulkan_data = nullptr;
    }
}
#endif
