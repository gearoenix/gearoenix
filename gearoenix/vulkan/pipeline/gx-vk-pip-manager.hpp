#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-singleton.hpp"
#include "../../core/gx-cr-static-flat-map.hpp"
#include "../gx-vk-loader.hpp"
#include "gx-vk-pip-format-pipelines.hpp"

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
private:
    std::shared_ptr<Cache> cache;
    std::unique_ptr<shader::Manager> shader_manager;

    std::shared_ptr<shader::Module> ray_gen_sm;
    std::shared_ptr<shader::Module> close_hit_sm;
    std::shared_ptr<shader::Module> shadow_miss_sm;
    std::shared_ptr<shader::Module> miss_sm;
    std::vector<vk::PipelineShaderStageCreateInfo> stages_create_info;
    std::vector<vk::RayTracingShaderGroupCreateInfoKHR> shader_group_create_info;

    std::shared_ptr<shader::Module> pbr_vert_sm;
    std::shared_ptr<shader::Module> pbr_frag_sm;
    std::shared_ptr<shader::Module> unlit_vert_sm;
    std::shared_ptr<shader::Module> unlit_frag_sm;
    std::shared_ptr<shader::Module> skybox_vert_sm;
    std::shared_ptr<shader::Module> skybox_equirectangular_frag_sm;
    std::shared_ptr<shader::Module> skybox_cube_frag_sm;
    std::shared_ptr<shader::Module> shadow_caster_vert_sm;

    std::shared_ptr<Pipeline> shadow;
    std::shared_ptr<Pipeline> skinned_shadow;

    core::static_flat_map<vk::Format, FormatPipelines, 4> formats_pipelines;

    void initialise_ray_tracing();
    void load_shaders();
    void initialise_shadow_pipelines();

    [[nodiscard]] std::shared_ptr<Pipeline> create_pbr_forward_pipeline(vk::Format colour_format);
    [[nodiscard]] std::shared_ptr<Pipeline> create_pbr_skinned_forward_pipeline(vk::Format colour_format);
    [[nodiscard]] std::shared_ptr<Pipeline> create_unlit_forward_pipeline(vk::Format colour_format);
    [[nodiscard]] std::shared_ptr<Pipeline> create_unlit_skinned_forward_pipeline(vk::Format colour_format);
    [[nodiscard]] std::shared_ptr<Pipeline> create_skybox_equirectangular_pipeline(vk::Format colour_format);
    [[nodiscard]] std::shared_ptr<Pipeline> create_skybox_cube_pipeline(vk::Format colour_format);
    [[nodiscard]] std::shared_ptr<Pipeline> create_shadow_pipeline(vk::Format depth_format);
    [[nodiscard]] std::shared_ptr<Pipeline> create_skinned_shadow_pipeline(vk::Format depth_format);
    [[nodiscard]] FormatPipelines create_format_pipelines(vk::Format colour_format);

public:
    [[nodiscard]] const std::shared_ptr<Cache>& get_cache() const { return cache; }
    [[nodiscard]] const std::unique_ptr<shader::Manager>& get_shader_manager() const { return shader_manager; }
    [[nodiscard]] const std::shared_ptr<Pipeline>& get_shadow() const { return shadow; }
    [[nodiscard]] const std::shared_ptr<Pipeline>& get_skinned_shadow() const { return skinned_shadow; }

    Manager(Manager&&) = delete;
    Manager(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;
    Manager& operator=(const Manager&) = delete;
    Manager();
    ~Manager() override;

    [[nodiscard]] const FormatPipelines& get_pipelines(vk::Format colour_format);

    [[nodiscard]] std::shared_ptr<Pipeline> create_ray_tracing_pbr(const std::shared_ptr<descriptor::SetLayout>& des_set_layout);
};
}
#endif
