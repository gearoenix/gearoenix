#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/reflection/gx-rnd-rfl-manager.hpp"
#include "../descriptor/gx-vk-des-uniform-indexer.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan::pipeline {
struct Cache;
struct Pipeline;
}

namespace gearoenix::vulkan::shader {
struct Module;
}

namespace gearoenix::vulkan::reflection {
using uniform_indexer_t = descriptor::UniformIndexer<GxShaderDataReflectionProbe>;
struct Runtime;
struct Manager final : render::reflection::Manager, core::Singleton<Manager> {
    GX_GET_CREF_PRV(uniform_indexer_t, uniform_indexer);

    GX_GET_VAL_PRV(VkDescriptorSetLayout, convolution_descriptor_set_layout, nullptr);
    GX_GET_VAL_PRV(VkSampler, convolution_sampler, nullptr);
    GX_GET_CREF_PRV(std::shared_ptr<pipeline::Cache>, convolution_pipeline_cache);

    GX_GET_VAL_PRV(VkPipelineLayout, irradiance_pipeline_layout, nullptr);
    GX_GET_CREF_PRV(std::shared_ptr<pipeline::Pipeline>, irradiance_pipeline);
    GX_GET_CREF_PRV(std::shared_ptr<shader::Module>, irradiance_shader_module);

    GX_GET_VAL_PRV(VkPipelineLayout, radiance_pipeline_layout, nullptr);
    GX_GET_CREF_PRV(std::shared_ptr<pipeline::Pipeline>, radiance_pipeline);
    GX_GET_CREF_PRV(std::shared_ptr<shader::Module>, radiance_shader_module);

    void initialise_convolution_compute();
    void update() override;

public:
    Manager();
    ~Manager() override;
    [[nodiscard]] core::ecs::EntityPtr build_baked(
        std::string&& name,
        core::ecs::Entity* parent,
        std::shared_ptr<render::texture::TextureCube>&& irradiance,
        std::shared_ptr<render::texture::TextureCube>&& radiance,
        const math::Aabb3<double>& include_box) override;
    void build_runtime(
        std::string&& name,
        core::ecs::Entity* parent,
        const math::Aabb3<double>& receive_box,
        const math::Aabb3<double>& exclude_box,
        const math::Aabb3<double>& include_box,
        std::uint32_t environment_resolution,
        std::uint32_t irradiance_resolution,
        std::uint32_t radiance_resolution,
        core::job::EndCaller<core::ecs::EntityPtr>&& entity_callback) override;
    void upload_uniforms();
    void submit(VkCommandBuffer cmd) const;
};
}
#endif
