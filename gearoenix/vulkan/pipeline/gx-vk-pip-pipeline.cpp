#include "gx-vk-pip-pipeline.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../descriptor/gx-vk-des-bindless.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "gx-vk-pip-cache.hpp"

gearoenix::vulkan::pipeline::Pipeline::Pipeline(std::shared_ptr<Layout>&& layout, std::shared_ptr<Cache>&& cache, vk::raii::Pipeline&& vulkan_data)
    : layout(std::move(layout))
    , cache(std::move(cache))
    , vulkan_data(std::move(vulkan_data))
{
}

gearoenix::vulkan::pipeline::Pipeline::~Pipeline() = default;

std::shared_ptr<gearoenix::vulkan::pipeline::Pipeline> gearoenix::vulkan::pipeline::Pipeline::construct_ray_tracing(
    std::shared_ptr<Layout>&& layout, std::shared_ptr<Cache>&& cache, const std::vector<vk::PipelineShaderStageCreateInfo>& stages_create_info, const std::vector<vk::RayTracingShaderGroupCreateInfoKHR>& shader_group_create_info)
{
    vk::RayTracingPipelineCreateInfoKHR info;
    info.setStages(stages_create_info);
    info.setGroups(shader_group_create_info);
    info.maxPipelineRayRecursionDepth = 2; // Ray depth
    info.layout = descriptor::Bindless::get().get_pipeline_layout();
    const auto& dev = device::Logical::get().get_device();
    auto result = dev.createRayTracingPipelineKHR(nullptr, cache->get_pipeline_cache(), info);
    return std::shared_ptr<Pipeline>(new Pipeline(std::move(layout), std::move(cache), std::move(result)));
}

std::shared_ptr<gearoenix::vulkan::pipeline::Pipeline> gearoenix::vulkan::pipeline::Pipeline::construct_graphics(std::shared_ptr<Cache>&& cache, const vk::GraphicsPipelineCreateInfo& create_info)
{
    const auto& dev = device::Logical::get().get_device();
    auto result = dev.createGraphicsPipeline(cache->get_pipeline_cache(), create_info);
    return std::shared_ptr<Pipeline>(new Pipeline(nullptr, std::move(cache), std::move(result)));
}

std::shared_ptr<gearoenix::vulkan::pipeline::Pipeline> gearoenix::vulkan::pipeline::Pipeline::construct_compute(std::shared_ptr<Cache>&& cache, const vk::ComputePipelineCreateInfo& create_info)
{
    const auto& dev = device::Logical::get().get_device();
    auto result = dev.createComputePipeline(cache->get_pipeline_cache(), create_info);
    return std::shared_ptr<Pipeline>(new Pipeline(nullptr, std::move(cache), std::move(result)));
}

#endif
