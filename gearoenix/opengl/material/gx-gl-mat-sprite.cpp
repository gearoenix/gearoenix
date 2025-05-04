#include "gx-gl-mat-sprite.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../core/allocator/gx-cr-alc-shared-array.hpp"
#include "../../render/record/gx-rnd-rcd-camera.hpp"
#include "../../render/record/gx-rnd-rcd-model.hpp"
#include "../gx-gl-engine.hpp"
#include "../gx-gl-mesh.hpp"
#include "../gx-gl-texture.hpp"
#include "../shader/gx-gl-shd-manager.hpp"
#include "../shader/gx-gl-shd-shadow-caster.hpp"
#include "../shader/gx-gl-shd-unlit.hpp"

gearoenix::gl::material::Sprite::Sprite(const std::string& name)
    : render::material::Sprite(name)
    , shadow_caster_combination(shader::Manager::get().get_combiner<shader::ShadowCasterCombination>())
    , unlit_combination(shader::Manager::get().get_combiner<shader::UnlitCombination>())
{
}

void gearoenix::gl::material::Sprite::construct(const std::string& name, core::job::EndCallerShared<render::material::Sprite>&& c)
{
    static auto allocator = core::allocator::SharedArray<Sprite, max_count>::construct();
    const auto result = allocator->make_shared(name);
    c.set_return(result);
    result->initialise(std::move(c));
}

gearoenix::gl::material::Sprite::~Sprite() = default;

void gearoenix::gl::material::Sprite::shadow(
    const Mesh& mesh,
    const render::record::Camera& camera,
    const render::record::CameraModel& camera_model,
    uint& current_shader)
{
    /// TODO: uv_transform must be used in here too
    auto& shadow_caster_shader = shadow_caster_combination->get(camera_model.model->bones_count);
    shadow_caster_shader.bind(current_shader);
    shadow_caster_shader.set_mvp_data(camera.mvps[camera_model.first_mvp_index].data());
    const math::Vec2 alpha_factor_alpha_cutoff(albedo_factor.w, alpha_cutoff);
    shadow_caster_shader.set_alpha_factor_alpha_cutoff_data(alpha_factor_alpha_cutoff.data());

    glActiveTexture(GL_TEXTURE0 + shadow_caster_shader.get_albedo_index());
    glBindTexture(GL_TEXTURE_2D, gl_albedo->get_object());

    glBindVertexArray(mesh.get_cached_vertex_object());
    glDrawElements(GL_TRIANGLES, mesh.get_cached_indices_count(), GL_UNSIGNED_INT, nullptr);
}

void gearoenix::gl::material::Sprite::render_forward(
    const Scene& scene,
    const render::record::Camera& camera,
    const render::record::CameraModel& camera_model,
    const Model& model,
    const Mesh& mesh,
    uint& current_shader)
{
    const auto& shader = unlit_combination->get(true, true, true, true);
    shader.bind(current_shader);
    shader.set_mvp_data(camera.mvps[camera_model.first_mvp_index].data());
    shader.set_albedo_factor_data(albedo_factor.data());
    shader.set_alpha_cutoff_data(&alpha_cutoff);
    shader.set_uv_transform_data(uv_transform.data());
    glActiveTexture(GL_TEXTURE0 + shader.get_albedo_index());
    glBindTexture(GL_TEXTURE_2D, gl_albedo->get_object());
    glBindVertexArray(mesh.get_cached_vertex_object());
    glDrawElements(GL_TRIANGLES, mesh.get_cached_indices_count(), GL_UNSIGNED_INT, nullptr);
}

void gearoenix::gl::material::Sprite::deferred_gbuffer_render(
    const Model&, const Mesh&, const Camera&, const Scene&, uint&)
{
    // Nothing should be rendered by this material
}

void gearoenix::gl::material::Sprite::set_albedo(std::shared_ptr<render::texture::Texture2D>&& t)
{
    gl_albedo = std::dynamic_pointer_cast<Texture2D>(t);
    render::material::Sprite::set_albedo(std::move(t));
}

#endif
