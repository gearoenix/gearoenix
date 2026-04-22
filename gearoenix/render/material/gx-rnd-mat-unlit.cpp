#include "gx-rnd-mat-unlit.hpp"
#include "../buffer/gx-rnd-buf-manager.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"
#include "gx-rnd-mat-uniform.hpp"

namespace {
using Mgr = gearoenix::render::buffer::Manager;
constexpr auto albedo_idx_off = offsetof(GxShaderDataMaterial, albedo_normal_texture_sampler_index);
// Unlit repurposes normal_metallic_factor to carry uv_transform, and writes alpha_cutoff into
// the .x of alpha_cutoff_occlusion_strength_reserved_reserved — match those layouts on upload.
constexpr auto uv_transform_off = offsetof(GxShaderDataMaterial, normal_metallic_factor);
constexpr auto alpha_cutoff_off = offsetof(GxShaderDataMaterial, alpha_cutoff_occlusion_strength_reserved_reserved);
}

gearoenix::render::material::Unlit::Unlit(const core::object_type_index_t final_type_index, std::string&& name)
    : Material(final_type_index, std::move(name), true)
    , albedo_factor(1.0f, 1.0f, 1.0f, 1.0f)
    , uv_transform(1.0f, 1.0f, 0.0f, 0.0f)
{
    const auto base = uniform.offset_from_frame_pointer;
    Mgr::upload_to_all_frames_uniforms(base + offsetof(GxShaderDataMaterial, albedo_factor), albedo_factor);
    Mgr::upload_to_all_frames_uniforms(base + uv_transform_off, uv_transform);
    Mgr::upload_to_all_frames_uniforms(base + alpha_cutoff_off, alpha_cutoff);
}

void gearoenix::render::material::Unlit::initialise(core::job::EndCallerShared<Unlit>&& c)
{
    texture::Manager::get().create_2d_from_colour(
        math::Vec4(1.0f, 1.0f, 1.0f, 1.0f),
        core::job::EndCallerShared<texture::Texture2D>([this, c](std::shared_ptr<texture::Texture2D>&& t) {
            set_albedo(std::move(t));
            (void)c;
        }));
}

gearoenix::render::material::Unlit::~Unlit() = default;

void gearoenix::render::material::Unlit::set_albedo(std::shared_ptr<texture::Texture2D>&& t)
{
    albedo = std::move(t);
    const std::uint32_t idx[4] = {
        albedo->get_shader_resource_index(),
        albedo->get_sampler_shader_resource_index(),
        0, 0
    };
    Mgr::upload_to_all_frames_uniforms(uniform.offset_from_frame_pointer + albedo_idx_off, idx, sizeof(idx));
}

void gearoenix::render::material::Unlit::set_albedo_factor(const math::Vec4<float>& v)
{
    albedo_factor = v;
    Mgr::upload_to_all_frames_uniforms(uniform.offset_from_frame_pointer + offsetof(GxShaderDataMaterial, albedo_factor), v);
}

void gearoenix::render::material::Unlit::set_uv_transform(const math::Vec4<float>& v)
{
    uv_transform = v;
    Mgr::upload_to_all_frames_uniforms(uniform.offset_from_frame_pointer + uv_transform_off, v);
}

void gearoenix::render::material::Unlit::set_alpha_cutoff(const float v)
{
    alpha_cutoff = v;
    Mgr::upload_to_all_frames_uniforms(uniform.offset_from_frame_pointer + alpha_cutoff_off, v);
}