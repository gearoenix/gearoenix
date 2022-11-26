#include "gx-gl-mat-sprite.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../gx-gl-engine.hpp"
#include "../gx-gl-texture.hpp"
#include "../shader/gx-gl-shd-manager.hpp"
#include "../shader/gx-gl-shd-shadow-caster.hpp"
#include "../shader/gx-gl-shd-unlit.hpp"
#include "../submission/gx-gl-sbm-camera.hpp"

gearoenix::gl::material::Sprite::Sprite(
    Engine& e,
    const std::string& name,
    const core::sync::EndCaller& c) noexcept
    : render::material::Sprite(e, name, c)
    , gl::material::Material(true)
    , gl_albedo(std::dynamic_pointer_cast<Texture2D>(albedo))
    , shadow_caster_combination(e.get_shader_manager()->get<shader::ShadowCasterCombination>())
    , unlit_combination(e.get_shader_manager()->get<shader::UnlitCombination>())
{
}

gearoenix::gl::material::Sprite::~Sprite() noexcept = default;

void gearoenix::gl::material::Sprite::shadow(
    const submission::Model& model,
    const submission::Camera& camera,
    uint& current_shader) noexcept
{
    /// TODO: uv_transform must be used in here too
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

void gearoenix::gl::material::Sprite::forward_render(
    const submission::Model& model,
    const submission::Camera& camera,
    const submission::Scene&,
    uint& current_shader) noexcept
{
    auto& shader = unlit_combination->get(true, true, true, true);
    shader.bind(current_shader);
    shader.set_mvp_data(reinterpret_cast<const float*>(&camera.mvps[model.fist_mvp_index]));
    shader.set_albedo_factor_data(reinterpret_cast<const float*>(&albedo_factor));
    shader.set_alpha_cutoff_data(reinterpret_cast<const float*>(&alpha_cutoff));
    shader.set_uv_transform_data(reinterpret_cast<const float*>(&uv_transform));
    glActiveTexture(GL_TEXTURE0 + shader.get_albedo_index());
    glBindTexture(GL_TEXTURE_2D, gl_albedo->get_object());
    glBindVertexArray(model.vertex_object);
    glDrawElements(GL_TRIANGLES, model.indices_count, GL_UNSIGNED_INT, nullptr);
}

void gearoenix::gl::material::Sprite::deferred_gbuffer_render(
    const submission::Model&,
    const submission::Camera&,
    const submission::Scene&,
    uint&) noexcept
{
}

void gearoenix::gl::material::Sprite::set_albedo(const std::shared_ptr<render::texture::Texture2D>& t) noexcept
{
    gl_albedo = std::dynamic_pointer_cast<Texture2D>(t);
    render::material::Sprite::set_albedo(t);
}

#endif
