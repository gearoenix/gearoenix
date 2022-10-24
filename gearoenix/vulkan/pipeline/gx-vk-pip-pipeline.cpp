#include "gx-vk-pip-pipeline.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../descriptor/gx-vk-des-set-layout.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../gx-vk-check.hpp"
#include "gx-vk-pip-cache.hpp"
#include "gx-vk-pip-layout.hpp"

gearoenix::vulkan::pipeline::Pipeline::Pipeline(
    std::shared_ptr<Layout> layout,
    std::shared_ptr<Cache> cache,
    VkPipeline vulkan_data) noexcept
    : layout(std::move(layout))
    , cache(std::move(cache))
    , vulkan_data(vulkan_data)
{
}

gearoenix::vulkan::pipeline::Pipeline::~Pipeline() noexcept
{
    vkDestroyPipeline(
        layout->get_des_set_layout()->get_logical_device().get_vulkan_data(), vulkan_data, nullptr);
}

std::shared_ptr<gearoenix::vulkan::pipeline::Pipeline> gearoenix::vulkan::pipeline::Pipeline::construct_ray_tracing(
    std::shared_ptr<Layout> layout,
    std::shared_ptr<Cache> cache,
    const std::vector<VkPipelineShaderStageCreateInfo>& stages_create_info,
    const std::vector<VkRayTracingShaderGroupCreateInfoKHR>& shader_group_create_info) noexcept
{
    VkRayTracingPipelineCreateInfoKHR info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
    info.stageCount = static_cast<std::uint32_t>(stages_create_info.size());
    info.pStages = stages_create_info.data();
    info.groupCount = static_cast<uint32_t>(shader_group_create_info.size());
    info.pGroups = shader_group_create_info.data();
    info.maxPipelineRayRecursionDepth = 2; // Ray depth
    info.layout = layout->get_vulkan_data();
    VkPipeline vulkan_data = nullptr;
    GX_VK_CHK(vkCreateRayTracingPipelinesKHR(
        layout->get_des_set_layout()->get_logical_device().get_vulkan_data(),
        nullptr, cache->get_vulkan_data(), 1, &info, nullptr, &vulkan_data));
    return std::shared_ptr<Pipeline>(new Pipeline(std::move(layout), std::move(cache), vulkan_data));
}

#endif