#include "gx-rnd-mat-pbr.hpp"

#include "../buffer/gx-rnd-buf-manager.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../imgui/gx-rnd-imgui-type-table.hpp"
#include "../imgui/gx-rnd-imgui-type-tree.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"
#include "gx-rnd-mat-uniform.hpp"

namespace {
using Mgr = gearoenix::render::buffer::Manager;
// Texture pairs occupy one uvec4 each; each setter writes only its 2-uint half.
constexpr auto albedo_normal_off = offsetof(GxShaderDataMaterial, albedo_normal_texture_sampler_index);
constexpr auto orm_emission_off = offsetof(GxShaderDataMaterial, orm_emission_texture_sampler_index);
constexpr auto half_uvec4 = sizeof(std::uint32_t) * 2;
}

gearoenix::render::material::Pbr::Pbr(const core::object_type_index_t final_type_index, std::string&& name)
    : Material(final_type_index, std::move(name), false)
    , albedo_factor(1.0f)
    , emission_roughness_factor(1.0f)
    , normal_metallic_factor(1.0f)
    , alpha_cutoff_occlusion_strength_reserved_reserved(math::Numeric::epsilon<float>, 1.0f, 0.0f, 0.0f)
{
    const auto base = uniform.offset_from_frame_pointer;
    Mgr::upload_to_all_frames_uniforms(base + offsetof(GxShaderDataMaterial, albedo_factor), albedo_factor);
    Mgr::upload_to_all_frames_uniforms(base + offsetof(GxShaderDataMaterial, emission_roughness_factor), emission_roughness_factor);
    Mgr::upload_to_all_frames_uniforms(base + offsetof(GxShaderDataMaterial, normal_metallic_factor), normal_metallic_factor);
    Mgr::upload_to_all_frames_uniforms(base + offsetof(GxShaderDataMaterial, alpha_cutoff_occlusion_strength_reserved_reserved), alpha_cutoff_occlusion_strength_reserved_reserved);
}

void gearoenix::render::material::Pbr::initialise(core::job::EndCallerShared<Pbr>&& c)
{
    auto& tm = texture::Manager::get();
    tm.create_2d_from_colour(math::Vec4(1.0f, 1.0f, 1.0f, 1.0f), core::job::EndCallerShared<texture::Texture2D>([this, c](std::shared_ptr<texture::Texture2D>&& t) {
        set_albedo(std::move(t));
        (void)c; // prevent this from deletion
    }));
    tm.create_2d_from_colour(math::Vec4(0.5f, 0.5f, 1.0f, 1.0f), core::job::EndCallerShared<texture::Texture2D>([this, c](std::shared_ptr<texture::Texture2D>&& t) {
        set_normal(std::move(t));
        (void)c; // prevent this from deletion
    }));
    tm.create_2d_from_colour(math::Vec4(0.0f, 0.0f, 0.0f, 1.0f), core::job::EndCallerShared<texture::Texture2D>([this, c](std::shared_ptr<texture::Texture2D>&& t) {
        set_emission(std::move(t));
        (void)c; // prevent this from deletion
    }));
    tm.create_2d_from_colour(math::Vec4(1.0f, 0.5f, 0.5f, 1.0f), core::job::EndCallerShared<texture::Texture2D>([this, c](std::shared_ptr<texture::Texture2D>&& t) {
        set_orm(std::move(t));
        (void)c; // prevent this from deletion
    }));
}

gearoenix::render::material::Pbr::~Pbr() = default;

void gearoenix::render::material::Pbr::set_albedo(std::shared_ptr<texture::Texture2D>&& o)
{
    albedo = std::move(o);
    const std::uint32_t idx[2] = { albedo->get_shader_resource_index(), albedo->get_sampler_shader_resource_index() };
    Mgr::upload_to_all_frames_uniforms(uniform.offset_from_frame_pointer + albedo_normal_off, idx, sizeof(idx));
}

void gearoenix::render::material::Pbr::show_debug_gui()
{
    imgui::tree_scope(this, [this] {
        Material::show_debug_gui();

        imgui::table_scope("##gearoenix::render::material::Pbr", [this] {
            constexpr ImVec2 image_size { 200.0f, 200.0f };

#define GX_MAT_HELPER(name, var)                                                   \
    ImGui::Text(name ": ");                                                        \
    ImGui::TableNextColumn();                                                      \
    ImGui::Image(reinterpret_cast<ImTextureID>(var->get_imgui_ptr()), image_size); \
    ImGui::TableNextColumn();

            GX_MAT_HELPER("Albedo", albedo);
            GX_MAT_HELPER("Normal", normal);
            GX_MAT_HELPER("ORM", orm);
            GX_MAT_HELPER("Emission", emission);
        });
    });
}

void gearoenix::render::material::Pbr::set_normal(std::shared_ptr<texture::Texture2D>&& o)
{
    normal = std::move(o);
    const std::uint32_t idx[2] = { normal->get_shader_resource_index(), normal->get_sampler_shader_resource_index() };
    Mgr::upload_to_all_frames_uniforms(uniform.offset_from_frame_pointer + albedo_normal_off + half_uvec4, idx, sizeof(idx));
}

void gearoenix::render::material::Pbr::set_emission(std::shared_ptr<texture::Texture2D>&& o)
{
    emission = std::move(o);
    const std::uint32_t idx[2] = { emission->get_shader_resource_index(), emission->get_sampler_shader_resource_index() };
    Mgr::upload_to_all_frames_uniforms(uniform.offset_from_frame_pointer + orm_emission_off + half_uvec4, idx, sizeof(idx));
}

void gearoenix::render::material::Pbr::set_orm(std::shared_ptr<texture::Texture2D>&& o)
{
    orm = std::move(o);
    const std::uint32_t idx[2] = { orm->get_shader_resource_index(), orm->get_sampler_shader_resource_index() };
    Mgr::upload_to_all_frames_uniforms(uniform.offset_from_frame_pointer + orm_emission_off, idx, sizeof(idx));
}

void gearoenix::render::material::Pbr::set_albedo_factor(const math::Vec4<float>& v)
{
    albedo_factor = v;
    Mgr::upload_to_all_frames_uniforms(uniform.offset_from_frame_pointer + offsetof(GxShaderDataMaterial, albedo_factor), v);
}

void gearoenix::render::material::Pbr::set_emission_roughness_factor(const math::Vec4<float>& v)
{
    emission_roughness_factor = v;
    Mgr::upload_to_all_frames_uniforms(uniform.offset_from_frame_pointer + offsetof(GxShaderDataMaterial, emission_roughness_factor), v);
}

void gearoenix::render::material::Pbr::set_normal_metallic_factor(const math::Vec4<float>& v)
{
    normal_metallic_factor = v;
    Mgr::upload_to_all_frames_uniforms(uniform.offset_from_frame_pointer + offsetof(GxShaderDataMaterial, normal_metallic_factor), v);
}

void gearoenix::render::material::Pbr::set_alpha_cutoff_occlusion_strength_reserved_reserved(const math::Vec4<float>& v)
{
    alpha_cutoff_occlusion_strength_reserved_reserved = v;
    Mgr::upload_to_all_frames_uniforms(uniform.offset_from_frame_pointer + offsetof(GxShaderDataMaterial, alpha_cutoff_occlusion_strength_reserved_reserved), v);
}