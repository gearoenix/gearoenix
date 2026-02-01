#include "gx-vk-mat-material.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../platform/gx-plt-log.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../descriptor/gx-vk-des-uniform-indexer.hpp"

gearoenix::vulkan::material::Material::Material()
    : shader_data(uniform_indexer_t::get().get_next())
{
}

gearoenix::vulkan::material::Material::~Material() = default;

void gearoenix::vulkan::material::Material::bind_forward(const VkCommandBuffer, const bool, VkPipeline&)
{
    GX_UNIMPLEMENTED; // needs to be implemented by the child type
}

void gearoenix::vulkan::material::Material::bind_shadow(const VkCommandBuffer, const bool, VkPipeline&)
{
    GX_UNIMPLEMENTED; // needs to be implemented by the child type
}

void gearoenix::vulkan::material::Material::bind_graphics(const VkPipeline pipeline, const VkCommandBuffer cmd, VkPipeline& current_bound_pipeline)
{
    if (current_bound_pipeline == pipeline) {
        return;
    }
    current_bound_pipeline = pipeline;
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}

#endif