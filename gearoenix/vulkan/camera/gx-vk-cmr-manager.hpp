#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/camera/gx-rnd-cmr-manager.hpp"
#include "../descriptor/gx-vk-des-uniform-indexer.hpp"
#include "../gx-vk-loader.hpp"

#include <memory>

namespace gearoenix::vulkan::pipeline {
struct Cache;
struct Pipeline;
}

namespace gearoenix::vulkan::shader {
struct Module;
}

namespace gearoenix::vulkan::camera {

struct BloomPushConstants final {
    float texel_size_x;
    float texel_size_y;
    float src_mip;
    float value;
    float scatter_clamp_max_threshold_threshold_knee[4];
};

static_assert(sizeof(BloomPushConstants) == 32);

struct ColourCorrectionPushConstants final {
    float mode;
    float param_x, param_y, param_z;
};

static_assert(sizeof(ColourCorrectionPushConstants) == 16);

struct Manager final : render::camera::Manager, core::Singleton<Manager> {
public:
    [[nodiscard]] const descriptor::UniformIndexer<GxShaderDataCamera>& get_camera_uniform_indexer() const { return camera_uniform_indexer; }
    [[nodiscard]] const descriptor::UniformIndexer<GxShaderDataCameraJointModel>& get_cameras_joint_models_uniform_indexer() const { return cameras_joint_models_uniform_indexer; }
    [[nodiscard]] vk::DescriptorSetLayout get_bloom_descriptor_set_layout() const { return *bloom_descriptor_set_layout; }
    [[nodiscard]] vk::Sampler get_bloom_sampler() const { return *bloom_sampler; }
    [[nodiscard]] vk::PipelineLayout get_bloom_pipeline_layout() const { return *bloom_pipeline_layout; }
    [[nodiscard]] const std::shared_ptr<pipeline::Pipeline>& get_bloom_multiply_pipeline() const { return bloom_multiply_pipeline; }
    [[nodiscard]] const std::shared_ptr<pipeline::Pipeline>& get_bloom_prefilter_pipeline() const { return bloom_prefilter_pipeline; }
    [[nodiscard]] const std::shared_ptr<pipeline::Pipeline>& get_bloom_horizontal_pipeline() const { return bloom_horizontal_pipeline; }
    [[nodiscard]] const std::shared_ptr<pipeline::Pipeline>& get_bloom_vertical_pipeline() const { return bloom_vertical_pipeline; }
    [[nodiscard]] const std::shared_ptr<pipeline::Pipeline>& get_bloom_upsampler_pipeline() const { return bloom_upsampler_pipeline; }
    [[nodiscard]] vk::PipelineLayout get_ctaa_pipeline_layout() const { return *ctaa_pipeline_layout; }
    [[nodiscard]] const std::shared_ptr<pipeline::Pipeline>& get_ctaa_pipeline() const { return ctaa_pipeline; }

private:
    descriptor::UniformIndexer<GxShaderDataCamera> camera_uniform_indexer;
    descriptor::UniformIndexer<GxShaderDataCameraJointModel> cameras_joint_models_uniform_indexer;

    vk::raii::DescriptorSetLayout bloom_descriptor_set_layout { nullptr };
    vk::raii::Sampler bloom_sampler { nullptr };
    vk::raii::PipelineLayout bloom_pipeline_layout { nullptr };
    std::shared_ptr<pipeline::Pipeline> bloom_multiply_pipeline;
    std::shared_ptr<pipeline::Pipeline> bloom_prefilter_pipeline;
    std::shared_ptr<pipeline::Pipeline> bloom_horizontal_pipeline;
    std::shared_ptr<pipeline::Pipeline> bloom_vertical_pipeline;
    std::shared_ptr<pipeline::Pipeline> bloom_upsampler_pipeline;

    vk::raii::PipelineLayout ctaa_pipeline_layout { nullptr };
    std::shared_ptr<pipeline::Pipeline> ctaa_pipeline;

    std::shared_ptr<pipeline::Cache> bloom_pipeline_cache;
    std::shared_ptr<shader::Module> bloom_multiply_shader_module;
    std::shared_ptr<shader::Module> bloom_prefilter_shader_module;
    std::shared_ptr<shader::Module> bloom_horizontal_shader_module;
    std::shared_ptr<shader::Module> bloom_vertical_shader_module;
    std::shared_ptr<shader::Module> bloom_upsampler_shader_module;
    std::shared_ptr<shader::Module> ctaa_shader_module;

    void initialise_bloom();
    void terminate_bloom();
    void initialise_ctaa();
    void terminate_ctaa();

public:
    Manager();
    ~Manager() override;
    void build(std::string&& name, core::ecs::Entity* parent, core::job::EndCaller<core::ecs::EntityPtr>&& entity_callback) override;
    void window_resized() override;
    void update() override;
    void upload_uniforms();
};
}

#endif
