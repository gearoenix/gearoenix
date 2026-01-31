#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"
#include <memory>
#include <vector>

namespace gearoenix::vulkan::pipeline {
struct Cache;
struct Layout;
struct Pipeline final {
    GX_GET_CREF_PRV(std::shared_ptr<Layout>, layout);
    GX_GET_CREF_PRV(std::shared_ptr<Cache>, cache);
    GX_GET_VAL_PRV(VkPipeline, vulkan_data, nullptr);

    Pipeline(std::shared_ptr<Layout>&& , std::shared_ptr<Cache> &&, VkPipeline);

public:
    Pipeline(Pipeline&&) = delete;
    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(Pipeline&&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;
    ~Pipeline();
    [[nodiscard]] static std::shared_ptr<Pipeline> construct_ray_tracing(
        std::shared_ptr<Layout>&& layout,
        std::shared_ptr<Cache>&& cache,
        const std::vector<VkPipelineShaderStageCreateInfo>& stages_create_info,
        const std::vector<VkRayTracingShaderGroupCreateInfoKHR>& shader_group_create_info);
    [[nodiscard]] static std::shared_ptr<Pipeline> construct_graphics(
        std::shared_ptr<Cache>&& cache, const VkGraphicsPipelineCreateInfo& create_info);
};
}
#endif
