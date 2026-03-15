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

struct Manager final : render::camera::Manager, core::Singleton<Manager> {
    GX_GET_CREF_PRV(descriptor::UniformIndexer<GxShaderDataCamera>, camera_uniform_indexer);
    GX_GET_CREF_PRV(descriptor::UniformIndexer<GxShaderDataCameraJointModel>, cameras_joint_models_uniform_indexer);

    GX_GET_VAL_PRV(VkDescriptorSetLayout, bloom_descriptor_set_layout, nullptr);
    GX_GET_VAL_PRV(VkSampler, bloom_sampler, nullptr);
    GX_GET_VAL_PRV(VkPipelineLayout, bloom_pipeline_layout, nullptr);
    GX_GET_CREF_PRV(std::shared_ptr<pipeline::Pipeline>, bloom_multiply_pipeline);
    GX_GET_CREF_PRV(std::shared_ptr<pipeline::Pipeline>, bloom_prefilter_pipeline);
    GX_GET_CREF_PRV(std::shared_ptr<pipeline::Pipeline>, bloom_horizontal_pipeline);
    GX_GET_CREF_PRV(std::shared_ptr<pipeline::Pipeline>, bloom_vertical_pipeline);
    GX_GET_CREF_PRV(std::shared_ptr<pipeline::Pipeline>, bloom_upsampler_pipeline);

    std::shared_ptr<pipeline::Cache> bloom_pipeline_cache;
    std::shared_ptr<shader::Module> bloom_multiply_shader_module;
    std::shared_ptr<shader::Module> bloom_prefilter_shader_module;
    std::shared_ptr<shader::Module> bloom_horizontal_shader_module;
    std::shared_ptr<shader::Module> bloom_vertical_shader_module;
    std::shared_ptr<shader::Module> bloom_upsampler_shader_module;

    void initialise_bloom();
    void terminate_bloom();

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
