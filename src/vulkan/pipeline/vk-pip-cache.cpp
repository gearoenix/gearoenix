#include "vk-pip-cache.hpp"
#ifdef USE_VULKAN
#include "../../core/cr-static.hpp"
#include "../device/vk-dev-logical.hpp"
#include "../device/vk-dev-physical.hpp"
#include "../vk-check.hpp"
#include "../vk-instance.hpp"

gearoenix::render::pipeline::Cache::Cache(device::Logical* logical_device)
    : logical_device(logical_device)
{
    const device::Physical* p = logical_device->get_physical_device();
    const Linker* l = p->get_instance()->get_linker();
    VkPipelineCacheCreateInfo pipeline_cache_create_info;
    setz(pipeline_cache_create_info);
    pipeline_cache_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    VKC(l->vkCreatePipelineCache(logical_device->get_vulkan_data(),
        &pipeline_cache_create_info, nullptr,
        &vulkan_data));
}

gearoenix::render::pipeline::Cache::~Cache()
{
    const device::Physical* p = logical_device->get_physical_device();
    const Linker* l = p->get_instance()->get_linker();
    l->vkDestroyPipelineCache(logical_device->get_vulkan_data(), vulkan_data,
        nullptr);
}

const gearoenix::render::device::Logical* gearoenix::render::pipeline::Cache::get_logical_device() const
{
    return logical_device;
}

gearoenix::render::device::Logical* gearoenix::render::pipeline::Cache::get_logical_device()
{
    return logical_device;
}

const VkPipelineCache& gearoenix::render::pipeline::Cache::get_vulkan_data()
    const
{
    return vulkan_data;
}
#endif
