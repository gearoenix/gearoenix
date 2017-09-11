#include "vk-pip-cache.hpp"
#include "../device/vk-dev-logical.hpp"
#include "../device/vk-dev-physical.hpp"
#include "../vk-instance.hpp"
#include "../../core/cr-static.hpp"
#include "../vk-check.hpp"
gearoenix::render::pipeline::Cache::Cache(const std::shared_ptr<device::Logical> &logical_device): logical_device(logical_device) {
    auto &l = logical_device->get_physical_device()->get_instance()->get_linker();
    VkPipelineCacheCreateInfo pipeline_cache_create_info;
    setz(pipeline_cache_create_info);
    pipeline_cache_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    VKC(l->vkCreatePipelineCache(logical_device->get_vulkan_data(), &pipeline_cache_create_info, nullptr, &vulkan_data));
}

gearoenix::render::pipeline::Cache::~Cache(){
    auto &l = logical_device->get_physical_device()->get_instance()->get_linker();
    l->vkDestroyPipelineCache(logical_device->get_vulkan_data(), vulkan_data, nullptr);
}

const std::shared_ptr<gearoenix::render::device::Logical> &gearoenix::render::pipeline::Cache::get_logical_device() const {
    return logical_device;
}

const VkPipelineCache &gearoenix::render::pipeline::Cache::get_vulkan_data() const {
    return vulkan_data;
}
