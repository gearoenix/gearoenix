#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-singleton.hpp"
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"

#include <memory>
#include <vector>

namespace gearoenix::vulkan::descriptor {
struct SetLayout;
}

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::vulkan::shader {
struct Manager;
struct Module;
}

namespace gearoenix::vulkan::pipeline {
struct Cache;
struct Pipeline;
struct Manager final : core::Singleton<Manager> {
    GX_GET_CREF_PRV(std::shared_ptr<Cache>, cache);
    GX_GET_UCPTR_PRV(shader::Manager, shader_manager);

    std::shared_ptr<shader::Module> ray_gen_sm;
    std::shared_ptr<shader::Module> close_hit_sm;
    std::shared_ptr<shader::Module> shadow_miss_sm;
    std::shared_ptr<shader::Module> miss_sm;
    std::vector<VkPipelineShaderStageCreateInfo> stages_create_info;
    std::vector<VkRayTracingShaderGroupCreateInfoKHR> shader_group_create_info;

    std::shared_ptr<shader::Module> pbr_vert_sm;
    std::shared_ptr<shader::Module> pbr_frag_sm;
    GX_GET_CREF_PRV(std::shared_ptr<Pipeline>, pbr_forward_pipeline);
    GX_GET_CREF_PRV(std::shared_ptr<Pipeline>, pbr_shadow_pipeline);
    GX_GET_CREF_PRV(std::shared_ptr<Pipeline>, pbr_skinned_forward_pipeline);
    GX_GET_CREF_PRV(std::shared_ptr<Pipeline>, pbr_skinned_shadow_pipeline);

    void initialise_ray_tracing();
    void initialise_rasterizer();

public:
    Manager(Manager&&) = delete;
    Manager(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;
    Manager& operator=(const Manager&) = delete;
    Manager();
    ~Manager() override;
    [[nodiscard]] std::shared_ptr<Pipeline> create_ray_tracing_pbr(const std::shared_ptr<descriptor::SetLayout>& des_set_layout);
};
}
#endif
