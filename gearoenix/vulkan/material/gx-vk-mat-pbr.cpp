#include "gx-vk-mat-pbr.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../pipeline/gx-vk-pip-format-pipelines.hpp"
#include "gx-vk-mat-draw-cache.hpp"

gearoenix::vulkan::material::Pbr::Pbr(std::string&& name)
    : render::material::Pbr(object_type_index, std::move(name))
{
}

void gearoenix::vulkan::material::Pbr::construct(std::string&& name, core::job::EndCallerShared<render::material::Pbr>&& c)
{
    auto result = Object::construct<Pbr>(std::move(name));
    auto& r = *result;
    c.set_return(std::move(result));
    r.initialise(std::move(c));
}

gearoenix::vulkan::material::Pbr::~Pbr() = default;

void gearoenix::vulkan::material::Pbr::set(const bool skinned, DrawCache& dc)
{
    if (skinned) {
        dc.forward_pipeline_index = static_cast<decltype(dc.forward_pipeline_index)>(pipeline::FormatPipelinesIndices::pbr_skinned_forward_index);
        dc.shadow_pipeline_index = static_cast<decltype(dc.shadow_pipeline_index)>(pipeline::ShadowPipelinesIndices::skinned_index);
    } else {
        dc.forward_pipeline_index = static_cast<decltype(dc.forward_pipeline_index)>(pipeline::FormatPipelinesIndices::pbr_forward_index);
        dc.shadow_pipeline_index = static_cast<decltype(dc.shadow_pipeline_index)>(pipeline::ShadowPipelinesIndices::no_skin_index);
    }
    dc.material_index = uniform.shader_data_index;
}

#endif