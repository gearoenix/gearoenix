#include "gx-gl-mat-unlit.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../core/allocator/gx-cr-alc-shared-array.hpp"
#include "../gx-gl-engine.hpp"
#include "../gx-gl-texture.hpp"
#include "../shader/gx-gl-shd-manager.hpp"
#include "../shader/gx-gl-shd-shadow-caster.hpp"
#include "../shader/gx-gl-shd-unlit.hpp"
#include "../submission/gx-gl-sbm-camera.hpp"

gearoenix::gl::material::Unlit::Unlit(Engine& e, const std::string& name)
    : render::material::Unlit(e, name)
    , gl::material::Material(true)
    , shadow_caster_combination(e.get_shader_manager()->get<shader::ShadowCasterCombination>())
    , unlit_combination(e.get_shader_manager()->get<shader::UnlitCombination>())
{
}

void gearoenix::gl::material::Unlit::construct(Engine& e, const std::string& name, core::job::EndCallerShared<render::material::Unlit>&& c)
{
    static core::allocator::SharedArray<gl::material::Unlit, render::material::Unlit::MAX_COUNT> allocator;
    auto result = allocator.make_shared(e, name);
    c.set_return(result);
    result->initialise(std::move(c));
}

gearoenix::gl::material::Unlit::~Unlit() = default;

void gearoenix::gl::material::Unlit::shadow(
    const submission::Model& model,
    const submission::Camera& camera,
    uint& current_shader)
{
    auto& shadow_caster_shader = shadow_caster_combination->get(model.bones_count);
    shadow_caster_shader.bind(current_shader);
    shadow_caster_shader.set_mvp_data(reinterpret_cast<const float*>(&camera.mvps[model.fist_mvp_index]));
    const math::Vec2<float> alpha_factor_alpha_cutoff(albedo_factor.w, alpha_cutoff);
    shadow_caster_shader.set_alpha_factor_alpha_cutoff_data(reinterpret_cast<const float*>(&alpha_factor_alpha_cutoff));

    glActiveTexture(GL_TEXTURE0 + shadow_caster_shader.get_albedo_index());
    glBindTexture(GL_TEXTURE_2D, gl_albedo->get_object());

    glBindVertexArray(model.vertex_object);
    glDrawElements(GL_TRIANGLES, model.indices_count, GL_UNSIGNED_INT, nullptr);
}

void gearoenix::gl::material::Unlit::forward_render(
    const submission::Model& model,
    const submission::Camera& camera,
    const submission::Scene&,
    uint& current_shader)
{
    auto& shader = unlit_combination->get(false, true, true, true);
    shader.bind(current_shader);
    shader.set_mvp_data(reinterpret_cast<const float*>(&camera.mvps[model.fist_mvp_index]));
    shader.set_albedo_factor_data(reinterpret_cast<const float*>(&albedo_factor));
    shader.set_alpha_cutoff_data(reinterpret_cast<const float*>(&alpha_cutoff));
    glActiveTexture(GL_TEXTURE0 + shader.get_albedo_index());
    glBindTexture(GL_TEXTURE_2D, gl_albedo->get_object());
    glBindVertexArray(model.vertex_object);
    glDrawElements(GL_TRIANGLES, model.indices_count, GL_UNSIGNED_INT, nullptr);
}

void gearoenix::gl::material::Unlit::deferred_gbuffer_render(
    const submission::Model&,
    const submission::Camera&,
    const submission::Scene&,
    uint&)
{
}

void gearoenix::gl::material::Unlit::set_albedo(std::shared_ptr<render::texture::Texture2D>&& t)
{
    gl_albedo = std::dynamic_pointer_cast<Texture2D>(t);
    render::material::Unlit::set_albedo(std::move(t));
}

#endif
