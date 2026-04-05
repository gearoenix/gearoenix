#include "gx-vk-mat-pbr.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../pipeline/gx-vk-pip-format-pipelines.hpp"
#include "../pipeline/gx-vk-pip-manager.hpp"
#include "../pipeline/gx-vk-pip-pipeline.hpp"
#include "../texture/gx-vk-txt-2d.hpp"
#include "gx-vk-mat-manager.hpp"
#include "gx-vk-mat-draw-cache.hpp"

gearoenix::vulkan::material::Pbr::Pbr(std::string&& name)
    : render::material::Pbr(object_type_index, std::move(name))
{
    auto& sd = *shader_data.get_ptr();
    sd.albedo_factor = albedo_factor;
    sd.emission_roughness_factor = emission_roughness_factor;
    sd.normal_metallic_factor = normal_metallic_factor;
    sd.alpha_cutoff_occlusion_strength_reserved = alpha_cutoff_occlusion_strength_reserved_reserved;
}

void gearoenix::vulkan::material::Pbr::construct(std::string&& name, core::job::EndCallerShared<render::material::Pbr>&& c)
{
    auto result = Object::construct<Pbr>(std::move(name));
    auto& r = *result;
    c.set_return(std::move(result));
    r.initialise(std::move(c));
}

gearoenix::vulkan::material::Pbr::~Pbr() = default;

void gearoenix::vulkan::material::Pbr::show_debug_gui()
{
    render::material::Pbr::show_debug_gui();
}

void gearoenix::vulkan::material::Pbr::set_albedo(std::shared_ptr<render::texture::Texture2D>&& txt)
{
    const auto& t = *core::cast_ptr<texture::Texture2D>(txt.get());

    auto& sd = *shader_data.get_ptr();
    sd.albedo_texture_index = t.get_view_index();
    sd.albedo_sampler_index = t.get_sampler_index();

    render::material::Pbr::set_albedo(std::move(txt));
}

void gearoenix::vulkan::material::Pbr::set_normal(std::shared_ptr<render::texture::Texture2D>&& txt)
{
    const auto& t = *core::cast_ptr<texture::Texture2D>(txt.get());

    auto& sd = *shader_data.get_ptr();
    sd.normal_texture_index = t.get_view_index();
    sd.normal_sampler_index = t.get_sampler_index();

    render::material::Pbr::set_normal(std::move(txt));
}

void gearoenix::vulkan::material::Pbr::set_emission(std::shared_ptr<render::texture::Texture2D>&& txt)
{
    const auto& t = *core::cast_ptr<texture::Texture2D>(txt.get());

    auto& sd = *shader_data.get_ptr();
    sd.emission_texture_index = t.get_view_index();
    sd.emission_sampler_index = t.get_sampler_index();

    render::material::Pbr::set_emission(std::move(txt));
}

void gearoenix::vulkan::material::Pbr::set_orm(std::shared_ptr<render::texture::Texture2D>&& txt)
{
    const auto& t = *core::cast_ptr<texture::Texture2D>(txt.get());

    auto& sd = *shader_data.get_ptr();
    sd.orm_texture_index = t.get_view_index();
    sd.orm_sampler_index = t.get_sampler_index();

    render::material::Pbr::set_orm(std::move(txt));
}

void gearoenix::vulkan::material::Pbr::set_albedo_factor(const math::Vec4<float>& v)
{
    render::material::Pbr::set_albedo_factor(v);
    auto& sd = *shader_data.get_ptr();
    sd.albedo_factor = v;
}

void gearoenix::vulkan::material::Pbr::set_emission_roughness_factor(const math::Vec4<float>& v)
{
    render::material::Pbr::set_emission_roughness_factor(v);
    auto& sd = *shader_data.get_ptr();
    sd.emission_roughness_factor = v;
}

void gearoenix::vulkan::material::Pbr::set_normal_metallic_factor(const math::Vec4<float>& v)
{
    render::material::Pbr::set_normal_metallic_factor(v);
    auto& sd = *shader_data.get_ptr();
    sd.normal_metallic_factor = v;
}

void gearoenix::vulkan::material::Pbr::set_alpha_cutoff_occlusion_strength_reserved_reserved(const math::Vec4<float>& v)
{
    render::material::Pbr::set_alpha_cutoff_occlusion_strength_reserved_reserved(v);
    auto& sd = *shader_data.get_ptr();
    sd.alpha_cutoff_occlusion_strength_reserved = v;
}

void gearoenix::vulkan::material::Pbr::set(const bool skinned, DrawCache& dc)
{
    if (skinned) {
        dc.forward_pipeline_index = static_cast<decltype(dc.forward_pipeline_index)>(pipeline::FormatPipelinesIndices::pbr_skinned_forward_index);
        dc.shadow_pipeline_index = static_cast<decltype(dc.shadow_pipeline_index)>(pipeline::ShadowPipelinesIndices::skinned_index);
    } else {
        dc.forward_pipeline_index = static_cast<decltype(dc.forward_pipeline_index)>(pipeline::FormatPipelinesIndices::pbr_forward_index);
        dc.shadow_pipeline_index = static_cast<decltype(dc.shadow_pipeline_index)>(pipeline::ShadowPipelinesIndices::no_skin_index);
    }
    dc.material_index = shader_data.get_index();
}

#endif