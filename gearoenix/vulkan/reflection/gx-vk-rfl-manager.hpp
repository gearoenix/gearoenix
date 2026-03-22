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
private:
    uniform_indexer_t uniform_indexer;

    vk::raii::DescriptorSetLayout convolution_descriptor_set_layout { nullptr };
    vk::raii::Sampler convolution_sampler { nullptr };
    std::shared_ptr<pipeline::Cache> convolution_pipeline_cache;

    vk::raii::PipelineLayout irradiance_pipeline_layout { nullptr };
    std::shared_ptr<pipeline::Pipeline> irradiance_pipeline;
    std::shared_ptr<shader::Module> irradiance_shader_module;

    vk::raii::PipelineLayout radiance_pipeline_layout { nullptr };
    std::shared_ptr<pipeline::Pipeline> radiance_pipeline;
    std::shared_ptr<shader::Module> radiance_shader_module;

public:
    [[nodiscard]] const uniform_indexer_t& get_uniform_indexer() const { return uniform_indexer; }

    [[nodiscard]] vk::DescriptorSetLayout get_convolution_descriptor_set_layout() const { return *convolution_descriptor_set_layout; }
    [[nodiscard]] vk::Sampler get_convolution_sampler() const { return *convolution_sampler; }
    [[nodiscard]] const std::shared_ptr<pipeline::Cache>& get_convolution_pipeline_cache() const { return convolution_pipeline_cache; }

    [[nodiscard]] vk::PipelineLayout get_irradiance_pipeline_layout() const { return *irradiance_pipeline_layout; }
    [[nodiscard]] const std::shared_ptr<pipeline::Pipeline>& get_irradiance_pipeline() const { return irradiance_pipeline; }
    [[nodiscard]] const std::shared_ptr<shader::Module>& get_irradiance_shader_module() const { return irradiance_shader_module; }

    [[nodiscard]] vk::PipelineLayout get_radiance_pipeline_layout() const { return *radiance_pipeline_layout; }
    [[nodiscard]] const std::shared_ptr<pipeline::Pipeline>& get_radiance_pipeline() const { return radiance_pipeline; }
    [[nodiscard]] const std::shared_ptr<shader::Module>& get_radiance_shader_module() const { return radiance_shader_module; }

private:
    void initialise_convolution_compute();
    void update() override;
    void initialise_black();

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
    void submit(vk::CommandBuffer cmd) const;
};
}
#endif
