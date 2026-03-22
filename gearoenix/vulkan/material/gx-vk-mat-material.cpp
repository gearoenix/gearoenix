#include "gx-vk-mat-material.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../platform/gx-plt-log.hpp"
#include "../descriptor/gx-vk-des-uniform-indexer.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../pipeline/gx-vk-pip-push-constant.hpp"

gearoenix::vulkan::material::Material::Material()
    : shader_data(uniform_indexer_t::get().get_next())
{
}

gearoenix::vulkan::material::Material::~Material() = default;

void gearoenix::vulkan::material::Material::bind_forward(const vk::CommandBuffer, const bool, const pipeline::FormatPipelines&, pipeline::PushConstants&, vk::Pipeline&)
{
    GX_UNIMPLEMENTED; // needs to be implemented by the child type
}

void gearoenix::vulkan::material::Material::bind_shadow(const vk::CommandBuffer, const bool, pipeline::PushConstants&, vk::Pipeline&)
{
    GX_UNIMPLEMENTED; // needs to be implemented by the child type
}

void gearoenix::vulkan::material::Material::bind_graphics(const vk::Pipeline pipeline, const vk::CommandBuffer cmd, pipeline::PushConstants& pc, vk::Pipeline& current_bound_pipeline)
{
    pc.material_index = shader_data.get_index();
    if (current_bound_pipeline != pipeline) {
        current_bound_pipeline = pipeline;
        cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
    }
}

#endif
