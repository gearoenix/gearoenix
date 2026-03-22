#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../gx-vk-loader.hpp"
#include <memory>
#include <vector>

namespace gearoenix::vulkan::pipeline {
struct Cache;
struct Layout;
struct Pipeline final {
private:
    const std::shared_ptr<Layout> layout;
    const std::shared_ptr<Cache> cache;
    vk::raii::Pipeline vulkan_data;

    Pipeline(std::shared_ptr<Layout>&&, std::shared_ptr<Cache>&&, vk::raii::Pipeline&&);

public:
    [[nodiscard]] const std::shared_ptr<Layout>& get_layout() const { return layout; }
    [[nodiscard]] const std::shared_ptr<Cache>& get_cache() const { return cache; }
    [[nodiscard]] vk::Pipeline get_vulkan_data() const { return *vulkan_data; }

    Pipeline(Pipeline&&) = delete;
    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(Pipeline&&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;
    ~Pipeline();
    [[nodiscard]] static std::shared_ptr<Pipeline> construct_ray_tracing(
        std::shared_ptr<Layout>&& layout, std::shared_ptr<Cache>&& cache, const std::vector<vk::PipelineShaderStageCreateInfo>& stages_create_info, const std::vector<vk::RayTracingShaderGroupCreateInfoKHR>& shader_group_create_info);
    [[nodiscard]] static std::shared_ptr<Pipeline> construct_graphics(std::shared_ptr<Cache>&& cache, const vk::GraphicsPipelineCreateInfo& create_info);
    [[nodiscard]] static std::shared_ptr<Pipeline> construct_compute(std::shared_ptr<Cache>&& cache, const vk::ComputePipelineCreateInfo& create_info);
};
}
#endif
