#include "gx-vk-mat-sprite.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../pipeline/gx-vk-pip-pipeline.hpp"
#include "../pipeline/gx-vk-pip-manager.hpp"
#include "../texture/gx-vk-txt-2d.hpp"
#include "gx-vk-mat-manager.hpp"

#include <atomic>

namespace {
std::atomic not_initialised = true;
gearoenix::vulkan::pipeline::Pipeline* forward_pipeline = nullptr;
VkPipeline vk_forward_pipeline = nullptr;
gearoenix::vulkan::pipeline::Pipeline* shadow_pipeline = nullptr;
VkPipeline vk_shadow_pipeline = nullptr;
gearoenix::vulkan::pipeline::Pipeline* skinned_forward_pipeline = nullptr;
VkPipeline vk_skinned_forward_pipeline = nullptr;
gearoenix::vulkan::pipeline::Pipeline* skinned_shadow_pipeline = nullptr;
VkPipeline vk_skinned_shadow_pipeline = nullptr;
}

gearoenix::vulkan::material::Sprite::Sprite(std::string&& name)
    : render::material::Sprite(object_type_index, std::move(name))
{
    auto& sd = *shader_data.get_ptr();
    sd.emission_roughness_factor = uv_transform;
    sd.albedo_factor = albedo_factor;
    sd.alpha_cutoff_occlusion_strength_reserved.x = alpha_cutoff;

    if (not_initialised.exchange(false, std::memory_order_relaxed)) {
        const auto& pip_mgr = pipeline::Manager::get();

        shadow_pipeline = pip_mgr.get_pbr_shadow_pipeline().get();
        forward_pipeline = pip_mgr.get_unlit_forward_pipeline().get();
        skinned_shadow_pipeline = pip_mgr.get_pbr_skinned_shadow_pipeline().get();
        skinned_forward_pipeline = pip_mgr.get_unlit_skinned_forward_pipeline().get();

        vk_shadow_pipeline = shadow_pipeline->get_vulkan_data();
        vk_forward_pipeline = forward_pipeline->get_vulkan_data();
        vk_skinned_shadow_pipeline = skinned_shadow_pipeline->get_vulkan_data();
        vk_skinned_forward_pipeline = skinned_forward_pipeline->get_vulkan_data();
    }
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

void gearoenix::vulkan::material::Sprite::bind_forward(const VkCommandBuffer cmd, const bool skinned, pipeline::PushConstants& pc, VkPipeline& current_bound_pipeline)
{
    bind_graphics(skinned ? vk_skinned_forward_pipeline : vk_forward_pipeline, cmd, pc, current_bound_pipeline);
}

void gearoenix::vulkan::material::Sprite::bind_shadow(const VkCommandBuffer cmd, const bool skinned, pipeline::PushConstants& pc, VkPipeline& current_bound_pipeline)
{
    bind_graphics(skinned ? vk_skinned_shadow_pipeline : vk_shadow_pipeline, cmd, pc, current_bound_pipeline);
}

#endif
