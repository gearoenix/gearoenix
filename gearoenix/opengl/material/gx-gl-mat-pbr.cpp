#include "gx-gl-mat-pbr.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../core/allocator/gx-cr-alc-shared-array.hpp"
#include "../../core/gx-cr-cast.hpp"
#include "../../physics/animation/gx-phs-anm-armature.hpp"
#include "../../physics/animation/gx-phs-anm-bone.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../../render/light/gx-rnd-lt-directional.hpp"
#include "../../render/light/gx-rnd-lt-light.hpp"
#include "../../render/record/gx-rnd-rcd-camera.hpp"
#include "../../render/record/gx-rnd-rcd-light.hpp"
#include "../../render/record/gx-rnd-rcd-model.hpp"
#include "../gx-gl-camera.hpp"
#include "../gx-gl-engine.hpp"
#include "../gx-gl-mesh.hpp"
#include "../gx-gl-reflection.hpp"
#include "../gx-gl-texture.hpp"
#include "../shader/gx-gl-shd-forward-pbr.hpp"
#include "../shader/gx-gl-shd-gbuffers-filler.hpp"
#include "../shader/gx-gl-shd-manager.hpp"
#include "../shader/gx-gl-shd-shadow-caster.hpp"

gearoenix::gl::material::Pbr::Pbr(const std::string& name)
    : render::material::Pbr(name)
    , shadow_caster_combination(shader::Manager::get().get_combiner<shader::ShadowCasterCombination>())
    , forward_pbr_combination(shader::Manager::get().get_combiner<shader::ForwardPbrCombination>())
    , gbuffers_filler_combination(Engine::get().get_specification().is_deferred_supported ? new shader::GBuffersFiller() : nullptr)
{
}

void gearoenix::gl::material::Pbr::construct(const std::string& name, core::job::EndCallerShared<render::material::Pbr>&& c)
{
    static const auto allocator = core::allocator::SharedArray<Pbr, max_count>::construct();
    const auto result = allocator->make_shared(name);
    c.set_return(result);
    result->initialise(std::move(c));
}

gearoenix::gl::material::Pbr::~Pbr() = default;

void gearoenix::gl::material::Pbr::shadow(
    const Mesh& mesh,
    const render::record::Camera& camera,
    const render::record::CameraModel& recorded_cam_model,
    uint& current_shader)
{
    auto& shadow_caster_shader = shadow_caster_combination->get(recorded_cam_model.model->bones_count);
    shadow_caster_shader.bind(current_shader);
    shadow_caster_shader.set_mvp_data(&camera.mvps[recorded_cam_model.first_mvp_index]);
    const math::Vec2 alpha_factor_alpha_cutoff(albedo_factor.w, alpha_cutoff_occlusion_strength_reserved_reserved.x);
    shadow_caster_shader.set_alpha_factor_alpha_cutoff_data(reinterpret_cast<const float*>(&alpha_factor_alpha_cutoff));

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

void gearoenix::gl::material::Pbr::set_metallic_roughness(std::shared_ptr<render::texture::Texture2D>&& o)
{
    gl_metallic_roughness = std::dynamic_pointer_cast<Texture2D>(o);
    render::material::Pbr::set_metallic_roughness(std::move(o));
}

void gearoenix::gl::material::Pbr::set_occlusion(std::shared_ptr<render::texture::Texture2D>&& o)
{
    gl_occlusion = std::dynamic_pointer_cast<Texture2D>(o);
    render::material::Pbr::set_occlusion(std::move(o));
}

void gearoenix::gl::material::Pbr::set_brdflut(std::shared_ptr<render::texture::Texture2D>&& o)
{
    gl_brdflut = std::dynamic_pointer_cast<Texture2D>(o);
    render::material::Pbr::set_brdflut(std::move(o));
}

void gearoenix::gl::material::Pbr::render_forward(
    const Scene& scene,
    const render::record::Camera& camera,
    const render::record::CameraModel& camera_model,
    const Model& model,
    const Mesh& mesh,
    uint& current_shader)
{
    const auto& rm = *camera_model.model;
    const auto& rls = rm.lights;
    auto& shader = forward_pbr_combination->get(
        rm.bones_count,
        static_cast<std::uint32_t>(rls.shadow_caster_directionals.size()),
        static_cast<std::uint32_t>(rls.directionals.size()));
    shader.bind(current_shader);
    shader.set_vp_data(camera.view_projection.data());
    shader.set_camera_position_reserved_data(camera.position.data());

    if (rm.bones_count > 0) {
        static std::vector<std::array<math::Mat4x4<float>, 2>> bones_data;
        bones_data.clear();
        for (const auto* const b : rm.armature->get_all_bones()) {
            bones_data.push_back({ math::Mat4x4<float>(b->get_global_matrix()),
                math::Mat4x4<float>(b->get_inverted_global_matrix()) });
        }
        shader.set_bones_m_inv_m_data(bones_data.data()->data());
    } else {
        const auto m = math::Mat4x4<float>(rm.transform->get_global_matrix());
        const auto inv_m = math::Mat4x4<float>(rm.transform->get_inverted_global_matrix());
        shader.set_m_data(m.data());
        shader.set_inv_m_data(inv_m.data());
    }
    shader.set_albedo_factor_data(reinterpret_cast<const float*>(&albedo_factor));
    shader.set_normal_metallic_factor_data(reinterpret_cast<const float*>(&normal_metallic_factor));
    shader.set_emission_roughness_factor_data(reinterpret_cast<const float*>(&emission_roughness_factor));
    alpha_cutoff_occlusion_strength_reserved_reserved.z = static_cast<float>(rm.probe->get_radiance_mips_count());
    shader.set_alpha_cutoff_occlusion_strength_radiance_lod_coefficient_reserved_data(alpha_cutoff_occlusion_strength_reserved_reserved.data());

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
    glBindTexture(GL_TEXTURE_CUBE_MAP, core::cast<const TextureCube>(rm.probe->get_irradiance().get())->get_object());
    glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_radiance_index()));
    glBindTexture(GL_TEXTURE_CUBE_MAP, core::cast<const TextureCube>(rm.probe->get_radiance().get())->get_object());
    glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_brdflut_index()));
    glBindTexture(GL_TEXTURE_2D, gl_brdflut->get_object());

    if (!rls.directionals.empty()) {
        boost::container::static_vector<math::Vec3<float>, GX_RENDER_MAX_DIRECTIONAL_LIGHTS> directionals_direction;
        boost::container::static_vector<math::Vec3<float>, GX_RENDER_MAX_DIRECTIONAL_LIGHTS> directionals_colour;
        for (const auto& [direction, light] : rls.directionals) {
            directionals_colour.emplace_back(light->colour);
            directionals_direction.emplace_back(direction);
        }
        shader.set_directional_light_direction_data(directionals_direction.data()->data());
        shader.set_directional_light_colour_data(directionals_colour.data()->data());
    }

    if (!rls.shadow_caster_directionals.empty()) {
        boost::container::static_vector<math::Mat4x4<float>, GX_RENDER_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER> lights_vp;
        boost::container::static_vector<math::Vec3<float>, GX_RENDER_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER> lights_direction;
        boost::container::static_vector<math::Vec3<float>, GX_RENDER_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER> lights_colour;

        for (const auto* const l : rls.shadow_caster_directionals) {
            lights_colour.emplace_back(l->colour);
            lights_direction.emplace_back(l->direction);
            lights_vp.emplace_back(l->normalised_vp);
        }

        shader.set_shadow_caster_directional_light_normalised_vp_data(lights_vp.data()->data());
        shader.set_shadow_caster_directional_light_direction_data(lights_direction.data()->data());
        shader.set_shadow_caster_directional_light_colour_data(lights_colour.data()->data());

        for (std::uint32_t ti = 0; ti < rls.shadow_caster_directionals.size(); ++ti) {
            glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_shadow_caster_directional_light_shadow_map_indices()[ti]));
            glBindTexture(GL_TEXTURE_2D, core::cast<const Texture2D>(rls.shadow_caster_directionals[ti]->shadow_caster_directional->get_shadow_map().get())->get_object());
        }
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
//     glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_metallic_roughness_index()));
//     glBindTexture(GL_TEXTURE_2D, gl_metallic_roughness->get_object());
//     glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_occlusion_index()));
//     glBindTexture(GL_TEXTURE_2D, gl_occlusion->get_object());
//     glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_irradiance_index()));
//     glBindTexture(GL_TEXTURE_CUBE_MAP, model.irradiance);
//     glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_radiance_index()));
//     glBindTexture(GL_TEXTURE_CUBE_MAP, model.radiance);
//     glBindVertexArray(mesh.vertex_object);
//     glDrawElements(GL_TRIANGLES, mesh.indices_count, GL_UNSIGNED_INT, nullptr);
// }

#endif
