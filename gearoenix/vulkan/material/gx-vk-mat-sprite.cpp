#include "gx-vk-mat-sprite.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../pipeline/gx-vk-pip-format-pipelines.hpp"
#include "../pipeline/gx-vk-pip-manager.hpp"
#include "../pipeline/gx-vk-pip-pipeline.hpp"
#include "../texture/gx-vk-txt-2d.hpp"
#include "gx-vk-mat-manager.hpp"

gearoenix::vulkan::material::Sprite::Sprite(std::string&& name)
    : render::material::Sprite(object_type_index, std::move(name))
{
    auto& sd = *shader_data.get_ptr();
    sd.emission_roughness_factor = uv_transform;
    sd.albedo_factor = albedo_factor;
    sd.alpha_cutoff_occlusion_strength_reserved.x = alpha_cutoff;
}

void gearoenix::vulkan::material::Sprite::construct(std::string&& name, core::job::EndCallerShared<render::material::Sprite>&& c)
{
    auto result = Object::construct<Sprite>(std::move(name));
    auto& r = *result;
    c.set_return(std::move(result));
    r.initialise(std::move(c));
}

gearoenix::vulkan::material::Sprite::~Sprite() = default;

void gearoenix::vulkan::material::Sprite::set_albedo(std::shared_ptr<render::texture::Texture2D>&& txt)
{
    const auto& t = *core::cast_ptr<texture::Texture2D>(txt.get());

    auto& sd = *shader_data.get_ptr();
    sd.albedo_texture_index = t.get_view_index();
    sd.albedo_sampler_index = t.get_sampler_index();

    render::material::Sprite::set_albedo(std::move(txt));
}

void gearoenix::vulkan::material::Sprite::set_albedo_factor(const math::Vec4<float>& v)
{
    render::material::Sprite::set_albedo_factor(v);
    auto& sd = *shader_data.get_ptr();
    sd.albedo_factor = v;
}

void gearoenix::vulkan::material::Sprite::set_uv_transform(const math::Vec4<float>& v)
{
    render::material::Sprite::set_uv_transform(v);
    auto& sd = *shader_data.get_ptr();
    sd.emission_roughness_factor = v;
}

void gearoenix::vulkan::material::Sprite::set_alpha_cutoff(const float v)
{
    render::material::Sprite::set_alpha_cutoff(v);
    auto& sd = *shader_data.get_ptr();
    sd.alpha_cutoff_occlusion_strength_reserved.x = v;
}

void gearoenix::vulkan::material::Sprite::bind_forward(const vk::CommandBuffer cmd, const bool skinned, const pipeline::FormatPipelines& fp, pipeline::PushConstants& pc, vk::Pipeline& current_bound_pipeline)
{
    bind_graphics((skinned ? fp.unlit_skinned_forward.get() : fp.unlit_forward.get())->get_vulkan_data(), cmd, pc, current_bound_pipeline);
}

void gearoenix::vulkan::material::Sprite::bind_shadow(const vk::CommandBuffer cmd, const bool skinned, pipeline::PushConstants& pc, vk::Pipeline& current_bound_pipeline)
{
    const auto& mgr = pipeline::Manager::get();
    bind_graphics((skinned ? mgr.get_skinned_shadow() : mgr.get_shadow())->get_vulkan_data(), cmd, pc, current_bound_pipeline);
}

#endif
