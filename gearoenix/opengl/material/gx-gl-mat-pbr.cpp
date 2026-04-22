#include "gx-gl-mat-pbr.hpp"
#if GX_RENDER_OPENGL_ENABLED
#include "../../core/gx-cr-cast.hpp"
#include "../../physics/animation/gx-phs-anm-armature.hpp"
#include "../../physics/animation/gx-phs-anm-bone.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../../render/light/gx-rnd-lt-directional.hpp"
#include "../../render/light/gx-rnd-lt-light.hpp"
#include "../../render/light/gx-rnd-lt-uniform.hpp"
#include "../../render/model/gx-rnd-mdl-model.hpp"
#include "../../render/model/gx-rnd-mdl-uniform.hpp"
#include "../../render/record/gx-rnd-rcd-camera.hpp"
#include "../../render/record/gx-rnd-rcd-model.hpp"
#include "../gx-gl-camera.hpp"
#include "../gx-gl-engine.hpp"
#include "../gx-gl-light.hpp"
#include "../gx-gl-mesh.hpp"
#include "../gx-gl-reflection.hpp"
#include "../gx-gl-scene.hpp"
#include "../gx-gl-texture.hpp"
#include "../shader/gx-gl-shd-forward-pbr.hpp"
#include "../shader/gx-gl-shd-gbuffers-filler.hpp"
#include "../shader/gx-gl-shd-manager.hpp"
#include "../shader/gx-gl-shd-shadow-caster.hpp"

gearoenix::gl::material::Pbr::Pbr(std::string&& name)
    : render::material::Pbr(object_type_index, std::move(name))
    , shadow_caster_combination(shader::Manager::get().get_combiner<shader::ShadowCasterCombination>())
    , forward_pbr_combination(shader::Manager::get().get_combiner<shader::ForwardPbrCombination>())
    , gbuffers_filler_combination(core::Singleton<Engine>::get().get_specification().is_deferred_supported ? new shader::GBuffersFiller() : nullptr)
{
}

void gearoenix::gl::material::Pbr::construct(std::string&& name, core::job::EndCallerShared<render::material::Pbr>&& c)
{
    auto result = Object::construct<Pbr>(std::move(name));
    auto& r = *result;
    c.set_return(std::move(result));
    r.initialise(std::move(c));
}

gearoenix::gl::material::Pbr::~Pbr() = default;

void gearoenix::gl::material::Pbr::shadow(const Mesh& mesh, const render::record::Camera&, const render::record::CameraModel& recorded_cam_model, uint& current_shader)
{
    auto& shadow_caster_shader = shadow_caster_combination->get(recorded_cam_model.model->model->get_bones_count());
    shadow_caster_shader.bind(current_shader);

    shadow_caster_shader.set_mvp_data(recorded_cam_model.get_first_mvp().mvp.data());

    const math::Vec2 alpha_factor_alpha_cutoff(albedo_factor.w, alpha_cutoff_occlusion_strength_reserved_reserved.x);
    shadow_caster_shader.set_alpha_factor_alpha_cutoff_data(alpha_factor_alpha_cutoff.data());

    glActiveTexture(GL_TEXTURE0 + shadow_caster_shader.get_albedo_index());
    glBindTexture(GL_TEXTURE_2D, gl_albedo->get_object());

    glBindVertexArray(mesh.get_cached_vertex_object());
    glDrawElements(GL_TRIANGLES, mesh.get_cached_indices_count(), GL_UNSIGNED_INT, nullptr);
}

void gearoenix::gl::material::Pbr::set_albedo(std::shared_ptr<render::texture::Texture2D>&& o)
{
    gl_albedo = std::dynamic_pointer_cast<Texture2D>(o);
    render::material::Pbr::set_albedo(std::move(o));
}

void gearoenix::gl::material::Pbr::set_normal(std::shared_ptr<render::texture::Texture2D>&& o)
{
    gl_normal = std::dynamic_pointer_cast<Texture2D>(o);
    render::material::Pbr::set_normal(std::move(o));
}

void gearoenix::gl::material::Pbr::set_emission(std::shared_ptr<render::texture::Texture2D>&& o)
{
    gl_emission = std::dynamic_pointer_cast<Texture2D>(o);
    render::material::Pbr::set_emission(std::move(o));
}

void gearoenix::gl::material::Pbr::set_orm(std::shared_ptr<render::texture::Texture2D>&& o)
{
    gl_orm = std::dynamic_pointer_cast<Texture2D>(o);
    render::material::Pbr::set_orm(std::move(o));
}

void gearoenix::gl::material::Pbr::render_forward(const Scene& scene, const render::record::Camera& camera, const render::record::CameraModel& camera_model, const Mesh& mesh, uint& current_shader)
{
    const auto& rm = *camera_model.model;
    const auto& rm_model_uniform = rm.model->get_uniform_inner_pointer();
    const auto [bones_count, point_lights_count, directional_lights_count, shadow_caster_directional_lights_count] = rm_model_uniform.bones_point_lights_directional_lights_shadow_caster_directional_lights_count;
    auto& shader = forward_pbr_combination->get(
        bones_count,
        static_cast<std::uint32_t>(shadow_caster_directional_lights_count),
        static_cast<std::uint32_t>(directional_lights_count));
    shader.bind(current_shader);
    shader.set_vp_data(camera.camera->get_view_projection().data());

    const math::Vec4 camera_position = { camera.transform->get_global_position().to<float>(), 1.0f };
    shader.set_camera_position_reserved_data(camera_position.data());

    if (bones_count > 0) {
        shader.set_bones_m_inv_m_data(rm.model->get_bones_uniform()->get_ref<GxShaderDataBone>().m.data());
    } else {
        shader.set_m_data(rm_model_uniform.m.data());
        shader.set_inv_m_data(rm_model_uniform.inv_transpose_m.data());
    }
    shader.set_albedo_factor_data(albedo_factor.data());
    shader.set_normal_metallic_factor_data(normal_metallic_factor.data());
    shader.set_emission_roughness_factor_data(emission_roughness_factor.data());
    alpha_cutoff_occlusion_strength_reserved_reserved.z = static_cast<float>(rm.probe->get_radiance_mips_count());
    shader.set_alpha_cutoff_occlusion_strength_radiance_lod_coefficient_reserved_data(alpha_cutoff_occlusion_strength_reserved_reserved.data());

    glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_albedo_index()));
    glBindTexture(GL_TEXTURE_2D, gl_albedo->get_object());
    glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_normal_index()));
    glBindTexture(GL_TEXTURE_2D, gl_normal->get_object());
    glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_emission_index()));
    glBindTexture(GL_TEXTURE_2D, gl_emission->get_object());
    glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_orm_index()));
    glBindTexture(GL_TEXTURE_2D, gl_orm->get_object());
    glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_irradiance_index()));
    glBindTexture(GL_TEXTURE_CUBE_MAP, core::cast_ptr<const TextureCube>(rm.probe->get_irradiance().get())->get_object());
    glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_radiance_index()));
    glBindTexture(GL_TEXTURE_CUBE_MAP, core::cast_ptr<const TextureCube>(rm.probe->get_radiance().get())->get_object());
    glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_brdflut_index()));
    glBindTexture(GL_TEXTURE_2D, core::cast_ptr<Texture2D>(scene.get_brdflut().get())->get_object());

    if (directional_lights_count) {
        std::array<math::Vec3<float>, GX_RENDER_MAX_DIRECTIONAL_LIGHTS> directionals_direction;
        std::array<math::Vec3<float>, GX_RENDER_MAX_DIRECTIONAL_LIGHTS> directionals_colour;
        for (std::uint32_t ti = 0; ti < directional_lights_count; ++ti) {
            const auto shader_index = rm_model_uniform.directional_lights[ti >> 2][ti & 3];
            const auto* const u = render::light::Directional::uniforms[shader_index];
            const auto& ub = u->get_ref<GxShaderDataDirectionalLight>();
            directionals_colour[ti] = ub.colour.xyz();
            directionals_direction[ti] = ub.direction.xyz();
        }
        shader.set_directional_light_direction_data(directionals_direction.data()->data());
        shader.set_directional_light_colour_data(directionals_colour.data()->data());
    }

    if (shadow_caster_directional_lights_count) {
        std::array<math::Mat4x4<float>, GX_RENDER_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER> lights_vps;
        std::array<math::Vec3<float>, GX_RENDER_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER> lights_directions;
        std::array<math::Vec3<float>, GX_RENDER_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER> lights_colours;

        for (std::uint32_t ti = 0; ti < shadow_caster_directional_lights_count; ++ti) {
            const auto shader_index = rm_model_uniform.shadow_caster_directional_lights[ti >> 2][ti & 3];
            const auto [sm, u] = ShadowCasterDirectionalLight::shadow_maps_uniforms[shader_index];
            glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_shadow_caster_directional_light_shadow_map_indices()[ti]));
            glBindTexture(GL_TEXTURE_2D, sm);
            const auto& ub = u->get_ref<GxShaderDataShadowCasterDirectionalLight>();
            lights_colours[ti] = ub.colour.xyz();
            lights_directions[ti] = ub.direction_bit_shadow_map_texture_index.xyz();
            lights_vps[ti] = ub.normalised_vp;
        }

        shader.set_shadow_caster_directional_light_normalised_vp_data(lights_vps.data()->data());
        shader.set_shadow_caster_directional_light_direction_data(lights_directions.data()->data());
        shader.set_shadow_caster_directional_light_colour_data(lights_colours.data()->data());
    }

    glBindVertexArray(mesh.get_cached_vertex_object());
    glDrawElements(GL_TRIANGLES, mesh.get_cached_indices_count(), GL_UNSIGNED_INT, nullptr);
}

// void gearoenix::gl::material::Pbr::deferred_gbuffer_render(
//     const Model& model,
//     const Mesh& mesh,
//     const Camera& camera,
//     const Scene&,
//     uint& current_shader)
// {
//     const auto& shader = *gbuffers_filler_combination;
//     shader.bind(current_shader);
//     shader.set_vp_data(reinterpret_cast<const float*>(&camera.vp));
//     shader.set_camera_position_data(reinterpret_cast<const float*>(&camera.pos));
//     shader.set_m_data(reinterpret_cast<const float*>(&model.m));
//     shader.set_inv_m_data(reinterpret_cast<const float*>(&model.inv_m));
//     shader.set_albedo_factor_data(reinterpret_cast<const float*>(&albedo_factor));
//     shader.set_normal_metallic_factor_data(reinterpret_cast<const float*>(&normal_metallic_factor));
//     shader.set_emission_roughness_factor_data(reinterpret_cast<const float*>(&emission_roughness_factor));
//     alpha_cutoff_occlusion_strength_reserved_reserved.z = model.radiance_lod_coefficient;
//     shader.set_alpha_cutoff_occlusion_strength_radiance_lod_coefficient_reserved_data(reinterpret_cast<const float*>(&alpha_cutoff_occlusion_strength_reserved_reserved));
//     glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_albedo_index()));
//     glBindTexture(GL_TEXTURE_2D, gl_albedo->get_object());
//     glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_normal_index()));
//     glBindTexture(GL_TEXTURE_2D, gl_normal->get_object());
//     glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_emission_index()));
//     glBindTexture(GL_TEXTURE_2D, gl_emission->get_object());
//     glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_orm_index()));
//     glBindTexture(GL_TEXTURE_2D, gl_orm->get_object());
//     glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_irradiance_index()));
//     glBindTexture(GL_TEXTURE_CUBE_MAP, model.irradiance);
//     glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_radiance_index()));
//     glBindTexture(GL_TEXTURE_CUBE_MAP, model.radiance);
//     glBindVertexArray(mesh.vertex_object);
//     glDrawElements(GL_TRIANGLES, mesh.indices_count, GL_UNSIGNED_INT, nullptr);
// }

#endif
