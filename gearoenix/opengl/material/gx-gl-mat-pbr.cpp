#include "gx-gl-mat-pbr.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../gx-gl-engine.hpp"
#include "../gx-gl-texture.hpp"
#include "../shader/gx-gl-shd-forward-pbr.hpp"
#include "../shader/gx-gl-shd-gbuffers-filler.hpp"
#include "../shader/gx-gl-shd-manager.hpp"
#include "../shader/gx-gl-shd-shadow-caster.hpp"
#include "../submission/gx-gl-sbm-camera.hpp"
#include "../submission/gx-gl-sbm-scene.hpp"

gearoenix::gl::material::Pbr::Pbr(Engine& e, const core::sync::EndCallerIgnored& c) noexcept
    : render::material::Pbr(e, c)
    , gl_albedo(std::dynamic_pointer_cast<Texture2D>(albedo))
    , gl_normal(std::dynamic_pointer_cast<Texture2D>(normal))
    , gl_emission(std::dynamic_pointer_cast<Texture2D>(emission))
    , gl_metallic_roughness(std::dynamic_pointer_cast<Texture2D>(metallic_roughness))
    , gl_occlusion(std::dynamic_pointer_cast<Texture2D>(occlusion))
    , gl_brdflut(std::dynamic_pointer_cast<Texture2D>(brdflut))
    , shadow_caster_combination(e.get_shader_manager()->get<shader::ShadowCasterCombination>())
    , forward_pbr_combination(e.get_shader_manager()->get<shader::ForwardPbrCombination>())
    , gbuffers_filler_combination(e.get_specification().is_deferred_supported ? new shader::GBuffersFiller(e) : nullptr)
{
}

gearoenix::gl::material::Pbr::~Pbr() noexcept = default;

void gearoenix::gl::material::Pbr::shadow(const submission::Model& model, const submission::Camera& camera, uint& current_shader) noexcept
{
    auto& shadow_caster_shader = shadow_caster_combination->get(model.bones_count);
    shadow_caster_shader.bind(current_shader);
    shadow_caster_shader.set_mvp_data(reinterpret_cast<const float*>(&camera.mvps[model.fist_mvp_index]));
    const math::Vec2<float> alpha_factor_alpha_cutoff(albedo_factor.w, alpha_cutoff_occlusion_strength_reserved_reserved.x);
    shadow_caster_shader.set_alpha_factor_alpha_cutoff_data(reinterpret_cast<const float*>(&alpha_factor_alpha_cutoff));

    glActiveTexture(GL_TEXTURE0 + shadow_caster_shader.get_albedo_index());
    glBindTexture(GL_TEXTURE_2D, gl_albedo->get_object());

    glBindVertexArray(model.vertex_object);
    glDrawElements(GL_TRIANGLES, model.indices_count, GL_UNSIGNED_INT, nullptr);
}

void gearoenix::gl::material::Pbr::set_albedo(std::shared_ptr<render::texture::Texture2D>&& o) noexcept
{
    gl_albedo = std::dynamic_pointer_cast<Texture2D>(o);
    render::material::Pbr::set_albedo(std::move(o));
}

void gearoenix::gl::material::Pbr::set_normal(std::shared_ptr<render::texture::Texture2D>&& o) noexcept
{
    gl_normal = std::dynamic_pointer_cast<Texture2D>(o);
    render::material::Pbr::set_normal(std::move(o));
}

void gearoenix::gl::material::Pbr::set_emission(std::shared_ptr<render::texture::Texture2D>&& o) noexcept
{
    gl_emission = std::dynamic_pointer_cast<Texture2D>(o);
    render::material::Pbr::set_emission(std::move(o));
}

void gearoenix::gl::material::Pbr::set_metallic_roughness(std::shared_ptr<render::texture::Texture2D>&& o) noexcept
{
    gl_metallic_roughness = std::dynamic_pointer_cast<Texture2D>(o);
    render::material::Pbr::set_metallic_roughness(std::move(o));
}

void gearoenix::gl::material::Pbr::set_occlusion(std::shared_ptr<render::texture::Texture2D>&& o) noexcept
{
    gl_occlusion = std::dynamic_pointer_cast<Texture2D>(o);
    render::material::Pbr::set_occlusion(std::move(o));
}

void gearoenix::gl::material::Pbr::forward_render(
    const submission::Model& model,
    const submission::Camera& camera,
    const submission::Scene& scene,
    uint& current_shader) noexcept
{

    auto& shader = forward_pbr_combination->get(model.bones_count, model.shadow_caster_directional_lights_count, model.directional_lights_count);
    shader.bind(current_shader);
    shader.set_vp_data(reinterpret_cast<const float*>(&camera.vp));
    shader.set_camera_position_reserved_data(reinterpret_cast<const float*>(&camera.pos));

    if (model.bones_count > 0) {
        shader.set_bones_m_inv_m_data(reinterpret_cast<const float*>(&scene.bones_data[model.first_bone_index]));
    } else {
        shader.set_m_data(reinterpret_cast<const float*>(&model.m));
        shader.set_inv_m_data(reinterpret_cast<const float*>(&model.inv_m));
    }
    shader.set_albedo_factor_data(reinterpret_cast<const float*>(&albedo_factor));
    shader.set_normal_metallic_factor_data(reinterpret_cast<const float*>(&normal_metallic_factor));
    shader.set_emission_roughness_factor_data(reinterpret_cast<const float*>(&emission_roughness_factor));
    alpha_cutoff_occlusion_strength_reserved_reserved.z = model.radiance_lod_coefficient;
    shader.set_alpha_cutoff_occlusion_strength_radiance_lod_coefficient_reserved_data(reinterpret_cast<const float*>(&alpha_cutoff_occlusion_strength_reserved_reserved));

    glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_albedo_index()));
    glBindTexture(GL_TEXTURE_2D, gl_albedo->get_object());
    glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_normal_index()));
    glBindTexture(GL_TEXTURE_2D, gl_normal->get_object());
    glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_emission_index()));
    glBindTexture(GL_TEXTURE_2D, gl_emission->get_object());
    glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_metallic_roughness_index()));
    glBindTexture(GL_TEXTURE_2D, gl_metallic_roughness->get_object());
    glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_occlusion_index()));
    glBindTexture(GL_TEXTURE_2D, gl_occlusion->get_object());
    glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_irradiance_index()));
    glBindTexture(GL_TEXTURE_CUBE_MAP, model.irradiance);
    glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_radiance_index()));
    glBindTexture(GL_TEXTURE_CUBE_MAP, model.radiance);
    glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_brdflut_index()));
    glBindTexture(GL_TEXTURE_2D, gl_brdflut->get_object());

    if (0 < model.directional_lights_count) {
        shader.set_directional_light_direction_data(model.directional_lights_direction.data());
        shader.set_directional_light_colour_data(model.directional_lights_colour.data());
    }

    if (0 < model.shadow_caster_directional_lights_count) {
        shader.set_shadow_caster_directional_light_normalised_vp_data(model.shadow_caster_directional_lights_normalised_vp.data());
        shader.set_shadow_caster_directional_light_direction_data(model.shadow_caster_directional_lights_direction.data());
        shader.set_shadow_caster_directional_light_colour_data(model.shadow_caster_directional_lights_colour.data());

        for (std::size_t ti = 0; ti < model.shadow_caster_directional_lights_count; ++ti) {
            glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_shadow_caster_directional_light_shadow_map_indices()[ti]));
            glBindTexture(GL_TEXTURE_2D, model.shadow_caster_directional_lights_shadow_map_texture[ti]);
        }
    }

    glBindVertexArray(model.vertex_object);
    glDrawElements(GL_TRIANGLES, model.indices_count, GL_UNSIGNED_INT, nullptr);
}

void gearoenix::gl::material::Pbr::deferred_gbuffer_render(
    const submission::Model& model,
    const submission::Camera& camera,
    const submission::Scene& scene,
    uint& current_shader) noexcept
{
    auto& shader = *gbuffers_filler_combination;
    shader.bind(current_shader);
    shader.set_vp_data(reinterpret_cast<const float*>(&camera.vp));
    shader.set_camera_position_data(reinterpret_cast<const float*>(&camera.pos));
    shader.set_m_data(reinterpret_cast<const float*>(&model.m));
    shader.set_inv_m_data(reinterpret_cast<const float*>(&model.inv_m));
    shader.set_albedo_factor_data(reinterpret_cast<const float*>(&albedo_factor));
    shader.set_normal_metallic_factor_data(reinterpret_cast<const float*>(&normal_metallic_factor));
    shader.set_emission_roughness_factor_data(reinterpret_cast<const float*>(&emission_roughness_factor));
    alpha_cutoff_occlusion_strength_reserved_reserved.z = model.radiance_lod_coefficient;
    shader.set_alpha_cutoff_occlusion_strength_radiance_lod_coefficient_reserved_data(reinterpret_cast<const float*>(&alpha_cutoff_occlusion_strength_reserved_reserved));
    glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_albedo_index()));
    glBindTexture(GL_TEXTURE_2D, gl_albedo->get_object());
    glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_normal_index()));
    glBindTexture(GL_TEXTURE_2D, gl_normal->get_object());
    glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_emission_index()));
    glBindTexture(GL_TEXTURE_2D, gl_emission->get_object());
    glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_metallic_roughness_index()));
    glBindTexture(GL_TEXTURE_2D, gl_metallic_roughness->get_object());
    glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_occlusion_index()));
    glBindTexture(GL_TEXTURE_2D, gl_occlusion->get_object());
    glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_irradiance_index()));
    glBindTexture(GL_TEXTURE_CUBE_MAP, model.irradiance);
    glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_radiance_index()));
    glBindTexture(GL_TEXTURE_CUBE_MAP, model.radiance);
    glBindVertexArray(model.vertex_object);
    glDrawElements(GL_TRIANGLES, model.indices_count, GL_UNSIGNED_INT, nullptr);
}

#endif
