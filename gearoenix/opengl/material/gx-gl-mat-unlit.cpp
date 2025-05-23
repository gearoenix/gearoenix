#include "gx-gl-mat-unlit.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../render/record/gx-rnd-rcd-camera.hpp"
#include "../../render/record/gx-rnd-rcd-model.hpp"
#include "../gx-gl-engine.hpp"
#include "../gx-gl-mesh.hpp"
#include "../gx-gl-texture.hpp"
#include "../shader/gx-gl-shd-manager.hpp"
#include "../shader/gx-gl-shd-shadow-caster.hpp"
#include "../shader/gx-gl-shd-unlit.hpp"

gearoenix::gl::material::Unlit::Unlit(std::string&& name)
    : render::material::Unlit(object_type_index, std::move(name))
    , shadow_caster_combination(shader::Manager::get().get_combiner<shader::ShadowCasterCombination>())
    , unlit_combination(shader::Manager::get().get_combiner<shader::UnlitCombination>())
{
}

void gearoenix::gl::material::Unlit::construct(std::string&& name, core::job::EndCallerShared<render::material::Unlit>&& c)
{
    const auto result = Object::construct<Unlit>(std::move(name));
    c.set_return(result);
    result->initialise(std::move(c));
}

gearoenix::gl::material::Unlit::~Unlit() = default;

void gearoenix::gl::material::Unlit::shadow(
    const Mesh& mesh,
    const render::record::Camera& cam,
    const render::record::CameraModel& cmm,
    uint& current_shader)
{
    auto& shadow_caster_shader = shadow_caster_combination->get(cmm.model->bones_count);
    shadow_caster_shader.bind(current_shader);
    shadow_caster_shader.set_mvp_data(cam.mvps[cmm.first_mvp_index].data());
    const math::Vec2 alpha_factor_alpha_cutoff(albedo_factor.w, alpha_cutoff);
    shadow_caster_shader.set_alpha_factor_alpha_cutoff_data(reinterpret_cast<const float*>(&alpha_factor_alpha_cutoff));

    glActiveTexture(GL_TEXTURE0 + shadow_caster_shader.get_albedo_index());
    glBindTexture(GL_TEXTURE_2D, gl_albedo->get_object());

    glBindVertexArray(mesh.get_cached_vertex_object());
    glDrawElements(GL_TRIANGLES, mesh.get_cached_indices_count(), GL_UNSIGNED_INT, nullptr);
}

void gearoenix::gl::material::Unlit::render_forward(
    const Scene&,
    const render::record::Camera& cam,
    const render::record::CameraModel& cmm,
    const Mesh& mesh,
    uint& current_shader)
{
    const auto& shader = unlit_combination->get(false, true, true, true);
    shader.bind(current_shader);
    shader.set_mvp_data(cam.mvps[cmm.first_mvp_index].data());
    shader.set_albedo_factor_data(albedo_factor.data());
    shader.set_alpha_cutoff_data(&alpha_cutoff);
    glActiveTexture(GL_TEXTURE0 + shader.get_albedo_index());
    glBindTexture(GL_TEXTURE_2D, gl_albedo->get_object());
    glBindVertexArray(mesh.get_cached_vertex_object());
    glDrawElements(GL_TRIANGLES, mesh.get_cached_indices_count(), GL_UNSIGNED_INT, nullptr);
}

void gearoenix::gl::material::Unlit::set_albedo(std::shared_ptr<render::texture::Texture2D>&& t)
{
    gl_albedo = std::dynamic_pointer_cast<Texture2D>(t);
    render::material::Unlit::set_albedo(std::move(t));
}

#endif
