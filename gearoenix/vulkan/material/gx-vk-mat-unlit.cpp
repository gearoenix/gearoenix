#include "gx-vk-mat-unlit.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../pipeline/gx-vk-pip-format-pipelines.hpp"
#include "gx-vk-mat-draw-cache.hpp"

gearoenix::vulkan::material::Unlit::Unlit(std::string&& name)
    : render::material::Unlit(object_type_index, std::move(name))
{
}

void gearoenix::vulkan::material::Unlit::construct(std::string&& name, core::job::EndCallerShared<render::material::Unlit>&& c)
{
    auto result = Object::construct<Unlit>(std::move(name));
    auto& r = *result;
    c.set_return(std::move(result));
    r.initialise(std::move(c));
}

gearoenix::vulkan::material::Unlit::~Unlit() = default;

void gearoenix::vulkan::material::Unlit::set(const bool skinned, DrawCache& dc)
{
    if (skinned) {
        dc.forward_pipeline_index = static_cast<decltype(dc.forward_pipeline_index)>(pipeline::FormatPipelinesIndices::unlit_skinned_forward_index);
        dc.shadow_pipeline_index = static_cast<decltype(dc.shadow_pipeline_index)>(pipeline::ShadowPipelinesIndices::skinned_index);
    } else {
        dc.forward_pipeline_index = static_cast<decltype(dc.forward_pipeline_index)>(pipeline::FormatPipelinesIndices::unlit_forward_index);
        dc.shadow_pipeline_index = static_cast<decltype(dc.shadow_pipeline_index)>(pipeline::ShadowPipelinesIndices::no_skin_index);
    }
    dc.material_index = uniform.shader_data_index;
}

#endif
