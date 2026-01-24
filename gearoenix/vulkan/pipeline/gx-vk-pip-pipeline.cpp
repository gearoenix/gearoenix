#include "gx-vk-pip-pipeline.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../gx-vk-check.hpp"
#include "../descriptor/gx-vk-des-bindless.hpp"
#include "gx-vk-pip-cache.hpp"

gearoenix::vulkan::pipeline::Pipeline::Pipeline(
    std::shared_ptr<Layout>&& layout,
    std::shared_ptr<Cache>&& cache,
    const VkPipeline vulkan_data)
    : layout(std::move(layout))
    , cache(std::move(cache))
    , vulkan_data(vulkan_data)
{
}

gearoenix::vulkan::pipeline::Pipeline::~Pipeline()
{
    vkDestroyPipeline(device::Logical::get().get_vulkan_data(), vulkan_data, nullptr);
}

std::shared_ptr<gearoenix::vulkan::pipeline::Pipeline> gearoenix::vulkan::pipeline::Pipeline::construct_ray_tracing(
    std::shared_ptr<Layout>&& layout,
    std::shared_ptr<Cache>&& cache,
    const std::vector<VkPipelineShaderStageCreateInfo>& stages_create_info,
    const std::vector<VkRayTracingShaderGroupCreateInfoKHR>& shader_group_create_info)
{
    VkRayTracingPipelineCreateInfoKHR info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
    info.stageCount = static_cast<std::uint32_t>(stages_create_info.size());
    info.pStages = stages_create_info.data();
    info.groupCount = static_cast<uint32_t>(shader_group_create_info.size());
    info.pGroups = shader_group_create_info.data();
    info.maxPipelineRayRecursionDepth = 2; // Ray depth
    info.layout = descriptor::Bindless::get().get_pipeline_layout();
    VkPipeline vulkan_data = nullptr;
    GX_VK_CHK(vkCreateRayTracingPipelinesKHR(device::Logical::get().get_vulkan_data(), nullptr, cache->get_vulkan_data(), 1, &info, nullptr, &vulkan_data));
    return std::shared_ptr<Pipeline>(new Pipeline(std::move(layout), std::move(cache), vulkan_data));
}

std::shared_ptr<gearoenix::vulkan::pipeline::Pipeline> gearoenix::vulkan::pipeline::Pipeline::construct_graphics(
    std::shared_ptr<Cache>&& cache, const VkGraphicsPipelineCreateInfo& create_info)
{
    VkPipeline vulkan_data = nullptr;
    GX_VK_CHK(vkCreateGraphicsPipelines(device::Logical::get().get_vulkan_data(), cache->get_vulkan_data(), 1, &create_info, nullptr, &vulkan_data));
    return std::shared_ptr<Pipeline>(new Pipeline(nullptr, std::move(cache), vulkan_data));
}

#endif