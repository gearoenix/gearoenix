#include "gx-gl-camera.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-world.hpp"
#include "../physics/gx-phs-transformation.hpp"
#include "../render/record/gx-rnd-rcd-camera.hpp"
#include "../render/record/gx-rnd-rcd-model.hpp"
#include "gx-gl-check.hpp"
#include "gx-gl-context.hpp"
#include "gx-gl-engine.hpp"
#include "gx-gl-label.hpp"
#include "gx-gl-model.hpp"
#include "gx-gl-scene.hpp"
#include "gx-gl-skybox.hpp"
#include "gx-gl-submission-manager.hpp"
#include "gx-gl-target.hpp"
#include "gx-gl-texture.hpp"
#include "shader/gx-gl-shd-bloom.hpp"
#include "shader/gx-gl-shd-ctaa.hpp"
#include "shader/gx-gl-shd-manager.hpp"
#include "shader/gx-gl-shd-multiply.hpp"
#include "shader/gx-gl-shd-skybox-cube.hpp"
#include "shader/gx-gl-shd-skybox-equirectangular.hpp"
#include <boost/mp11/algorithm.hpp>

gearoenix::gl::CameraTarget::~CameraTarget() = default;

gearoenix::gl::CameraTarget gearoenix::gl::CameraTarget::construct(const render::camera::Target& target)
{
    if (target.is_default()) {
        const auto& d = target.get_default();
        CameraTarget result { .target = Default {} };
        auto& gd = std::get<default_var_index>(result.target);
        gd.main = std::dynamic_pointer_cast<Target>(d.main);
        for (std::uint32_t ti = 0; ti < d.targets.size(); ++ti) {
            for (std::uint32_t mi = 0; mi < d.targets[ti].size(); ++mi) {
                gd.targets[ti][mi] = std::dynamic_pointer_cast<Target>(d.targets[ti][mi]);
                gd.framebuffers[ti][mi] = gd.targets[ti][mi]->get_framebuffer();
            }
            gd.colour_attachments[ti] = gd.targets[ti][0]->get_gl_attachments()[0].texture_object;
        }
        return result;
    }
    if (target.is_customised()) {
        const auto& d = target.get_customised();
        return CameraTarget { .target = Customised { .target = std::dynamic_pointer_cast<Target>(d.target) } };
    }
    GX_UNEXPECTED;
}

const gearoenix::gl::CameraTarget::Customised& gearoenix::gl::CameraTarget::get_customised() const
{
    GX_ASSERT_D(customised_var_index == target.index());
    return std::get<customised_var_index>(target);
}

const gearoenix::gl::CameraTarget::Default& gearoenix::gl::CameraTarget::get_default() const
{
    GX_ASSERT_D(default_var_index == target.index());
    return std::get<default_var_index>(target);
}

void gearoenix::gl::Camera::set_customised_target(std::shared_ptr<render::texture::Target>&& t)
{
    gl_target.target = CameraTarget::Customised { .target = std::dynamic_pointer_cast<Target>(t) };
    render::camera::Camera::set_customised_target(std::move(t));
}

void gearoenix::gl::Camera::update_target(core::job::EndCaller<>&& end)
{
    render::camera::Camera::update_target(core::job::EndCaller([this, self = object_self.lock(), end = std::move(end)] {
        (void)end;
        (void)self;
        gl_target = CameraTarget::construct(target);
    }));
}

gearoenix::gl::Camera::Camera(const std::string& name, render::camera::Target&& target, std::shared_ptr<physics::Transformation>&& transform)
    : render::camera::Camera(core::ecs::ComponentType::create_index(this), name, std::move(target), std::move(transform))
    , bloom_prefilter_shader(shader::Manager::get().get_shader<shader::BloomPrefilter>())
    , bloom_horizontal_shader(shader::Manager::get().get_shader<shader::BloomHorizontal>())
    , bloom_vertical_shader(shader::Manager::get().get_shader<shader::BloomVertical>())
    , bloom_upsampler_shader(shader::Manager::get().get_shader<shader::BloomUpsampler>())
    , multiply_shader(shader::Manager::get().get_shader<shader::Multiply>())
    , skybox_cube_shader(shader::Manager::get().get_shader<shader::SkyboxCube>())
    , skybox_equirectangular_shader(shader::Manager::get().get_shader<shader::SkyboxEquirectangular>())
    , colour_tuning_anti_aliasing_shader_combination(shader::Manager::get().get_combiner<shader::ColourTuningAntiAliasingCombination>())
{
}

void gearoenix::gl::Camera::construct(const std::string& name, core::job::EndCallerShared<Camera>&& c, std::shared_ptr<physics::Transformation>&& transform)
{
    c.set_return(Object::construct<Camera>(name, render::camera::Target(), std::move(transform)));
    c.get_return()->initialise();
    c.get_return()->update_target(core::job::EndCaller([c] {
        c.get_return()->enable_bloom();
    }));
}

gearoenix::gl::Camera::~Camera() = default;

void gearoenix::gl::Camera::render_shadow(const render::record::Camera& cmr, uint& current_shader) const
{
    push_debug_group(render_pass_name);
    ctx::set_framebuffer(gl_target.get_customised().target->get_framebuffer());
    ctx::set_viewport_scissor_clip(math::Vec4<sizei>(cmr.viewport_clip));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    for (auto& distance_model_data : cmr.all_models) {
        auto& camera_model = distance_model_data.second;
        auto& model = *core::cast<Model>(camera_model.model->model);
        model.render_shadow(cmr, camera_model, current_shader);
    }
    pop_debug_group();
}

void gearoenix::gl::Camera::render_forward(const Scene& scene, const render::record::Camera& cmr, uint& current_shader) const
{
#if GX_GL_LABELING_ENABLED
    static std::string debug_group;
    debug_group.clear();
    debug_group += "render-forward-camera for scene: ";
    debug_group += scene.get_object_name();
    debug_group += ", and for camera: ";
    debug_group += object_name;
#endif

    push_debug_group(debug_group);
    GX_GL_CHECK_D;
    if (target.is_customised()) {
        ctx::set_framebuffer(gl_target.get_customised().target->get_framebuffer());
    } else {
        ctx::set_framebuffer(gl_target.get_default().main->get_framebuffer());
    }
    ctx::set_viewport_scissor_clip(math::Vec4<sizei>(cmr.viewport_clip));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    render_forward_skyboxes(scene, cmr, current_shader);
    GX_GL_CHECK_D;
    glEnable(GL_BLEND); /// TODO: take it into a context and material must decide
    // Rendering forward pbr
    for (auto& distance_model_data : cmr.all_models) {
        auto& camera_model = distance_model_data.second;
        auto& model = *core::cast<Model>(camera_model.model->model);
        model.render_forward(scene, cmr, camera_model, current_shader);
        GX_GL_CHECK_D;
    }
    glDisable(GL_BLEND); /// TODO: take it into a context and material must decide
    pop_debug_group();
}

void gearoenix::gl::Camera::render_forward_skyboxes(const Scene& scene, const render::record::Camera& cmr, uint& current_shader) const
{
    static std::string debug_group;
    debug_group.clear();
    debug_group += "render-skyboxes for scene: ";
    debug_group += scene.get_object_name();
    debug_group += ", and for camera: ";
    debug_group += object_name;

    push_debug_group(debug_group);
    glDepthMask(GL_FALSE);
    // Rendering skyboxes
    const auto camera_pos_scale = math::Vec4(cmr.position, cmr.skybox_scale);
    bool is_equirectangular_current = true;
    skybox_equirectangular_shader->bind(current_shader);
    skybox_equirectangular_shader->set_vp_data(cmr.view_projection.data());
    skybox_equirectangular_shader->set_camera_position_box_scale_data(camera_pos_scale.data());
    auto skybox_texture_bind_index = static_cast<enumerated>(skybox_equirectangular_shader->get_albedo_index());
    for (const auto& distance_skybox : scene.get_record().skyboxes.skyboxes) {
        const auto& skybox = distance_skybox.second;
        const auto is_equirectangular = skybox.equirectangular;
        if (is_equirectangular != is_equirectangular_current) {
            is_equirectangular_current = is_equirectangular;
            if (is_equirectangular) {
                skybox_equirectangular_shader->bind(current_shader);
                skybox_equirectangular_shader->set_vp_data(cmr.view_projection.data());
                skybox_equirectangular_shader->set_camera_position_box_scale_data(camera_pos_scale.data());
                skybox_texture_bind_index = static_cast<enumerated>(skybox_equirectangular_shader->get_albedo_index());
            } else {
                skybox_cube_shader->bind(current_shader);
                skybox_cube_shader->set_vp_data(cmr.view_projection.data());
                skybox_cube_shader->set_camera_position_box_scale_data(camera_pos_scale.data());
                skybox_texture_bind_index = static_cast<enumerated>(skybox_cube_shader->get_albedo_index());
            }
        }
        glActiveTexture(GL_TEXTURE0 + skybox_texture_bind_index);
        const auto& gl_sky = *static_cast<const Skybox*>(skybox.skybox);
        const auto& gl_txt = gl_sky.get_gl_texture();
        constexpr auto texture_2d_index = boost::mp11::mp_find<Skybox::GlTexture, std::shared_ptr<Texture2D>>::value;
        constexpr auto texture_cube_index = boost::mp11::mp_find<Skybox::GlTexture, std::shared_ptr<TextureCube>>::value;
        if (is_equirectangular) {
            glBindTexture(GL_TEXTURE_2D, std::get<texture_2d_index>(gl_txt)->get_object());
        } else {
            glBindTexture(GL_TEXTURE_CUBE_MAP, std::get<texture_cube_index>(gl_txt)->get_object());
        }
        glBindVertexArray(gl_sky.get_vertex_object());
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
    }
    glDepthMask(GL_TRUE);
    pop_debug_group();
}

void gearoenix::gl::Camera::render_bloom(const Scene& scene, const render::record::Camera& record_cam, uint& current_shader) const
{
    if (!bloom_data.has_value()) {
        return;
    }

    // F[0][0] -> F[1][0] Multiply
    // F[1][0] -> F[0][1] Prefilter
    // F[0][1] -> F[1][1] HG -------------------- F[1][1] -> F[0][2] VG
    // F[0][2] -> F[1][2] HG -------------------- F[1][2] -> F[0][3] VG
    // F[0][3] -> F[1][3] HG -------------------- F[1][3] -> F[0][4] VG
    // F[0][4] -> F[1][4] HG -------------------- F[1][4] -> F[0][5] VG
    // F[0][5] -> F[1][5] HG -------------------- F[1][5] -> F[0][6] VG
    // F[0][6] -> F[1][6] HG -------------------- F[1][6] -> F[0][7] VG
    // F[0][7] -> F[1][7] Copy ------------------ F[1][7], F[1][6] -> F[0][6] US
    // F[0][6] -> F[1][6] Copy ------------------ F[1][6], F[1][5] -> F[0][5] US
    // F[0][5] -> F[1][5] Copy ------------------ F[1][5], F[1][4] -> F[0][4] US
    // F[0][4] -> F[1][4] Copy ------------------ F[1][4], F[1][3] -> F[0][3] US
    // F[0][3] -> F[1][3] Copy ------------------ F[1][3], F[1][2] -> F[0][2] US
    // F[0][2] -> F[1][2] Copy ------------------ F[1][2], F[1][1] -> F[0][1] US
    // F[0][1] -> F[1][1] Copy ------------------ F[1][1], F[1][0] -> F[0][0] US

    const auto& b = *bloom_data;

#if GX_GL_LABELING_ENABLED
    static std::string debug_group;
    debug_group.clear();
    debug_group += "render-bloom in scene: ";
    debug_group += scene.get_object_name();
    debug_group += ", and for camera: ";
    debug_group += object_name;
#endif

    push_debug_group(debug_group);

    GX_GL_CHECK_D;

    glDisable(GL_BLEND);

    GX_GL_CHECK_D;

    const auto viewport_clip = math::Vec4<sizei>(record_cam.viewport_clip);

    push_debug_group("copy");
    ctx::set_framebuffer(gl_target.get_default().framebuffers[1][0]);
    ctx::set_viewport_scissor_clip(viewport_clip);
    multiply_shader->bind(current_shader);
    {
        const math::Vec4 value_mip_index(math::Vec3(exposure.get_enabled() ? exposure.get_value() : 1.0f), 0.0f);
        multiply_shader->set_value_mip_index_data(value_mip_index.data());
    }
    glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(multiply_shader->get_source_texture_index()));
    glBindTexture(GL_TEXTURE_2D, gl_target.get_default().colour_attachments[0]);
    glBindVertexArray(submission::Manager::get().get_screen_vertex_object());
    glDrawArrays(GL_TRIANGLES, 0, 3);
    pop_debug_group();

    math::Vec3 texel_size_mip_index(math::Vec2(1.0f) / (record_cam.viewport_clip.zw() - record_cam.viewport_clip.xy()), 0.0f);

    push_debug_group("prefilter");
    GX_GL_CHECK_D;
    ctx::set_framebuffer(gl_target.get_default().framebuffers[0][1]);
    GX_GL_CHECK_D;
    ctx::set_viewport_scissor_clip(viewport_clip / 2);
    GX_GL_CHECK_D;
    bloom_prefilter_shader->bind(current_shader);
    GX_GL_CHECK_D;
    bloom_prefilter_shader->set_texel_size_data(texel_size_mip_index.data());
    GX_GL_CHECK_D;
    bloom_prefilter_shader->set_scatter_clamp_max_threshold_threshold_knee_data(b.get_scatter_clamp_max_threshold_threshold_knee().data());
    GX_GL_CHECK_D;
    glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(bloom_prefilter_shader->get_source_texture_index()));
    GX_GL_CHECK_D;
    glBindTexture(GL_TEXTURE_2D, gl_target.get_default().colour_attachments[1]);
    GX_GL_CHECK_D;
    glBindVertexArray(submission::Manager::get().get_screen_vertex_object());
    GX_GL_CHECK_D;
    glDrawArrays(GL_TRIANGLES, 0, 3);
    pop_debug_group();

    GX_GL_CHECK_D;

    for (int layer_index = 1; layer_index < gl_target.get_default().framebuffers[0].size() - 1; ++layer_index) {
        texel_size_mip_index = math::Vec3(texel_size_mip_index.xy() * 2.0f, static_cast<float>(layer_index));

        push_debug_group("horizontal-" + std::to_string(layer_index));
        ctx::set_framebuffer(gl_target.get_default().framebuffers[1][layer_index]);
        ctx::set_viewport_scissor_clip(viewport_clip / (1 << layer_index));
        bloom_horizontal_shader->bind(current_shader);
        bloom_horizontal_shader->set_texel_size_mip_index_data(texel_size_mip_index.data());
        glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(bloom_horizontal_shader->get_source_texture_index()));
        glBindTexture(GL_TEXTURE_2D, gl_target.get_default().colour_attachments[0]);
        glBindVertexArray(submission::Manager::get().get_screen_vertex_object());
        glDrawArrays(GL_TRIANGLES, 0, 3);
        pop_debug_group();

        const auto next_layer = layer_index + 1;
        push_debug_group("vertical-" + std::to_string(layer_index));
        ctx::set_framebuffer(gl_target.get_default().framebuffers[0][next_layer]);
        ctx::set_viewport_scissor_clip(viewport_clip / (1 << next_layer));
        bloom_vertical_shader->bind(current_shader);
        bloom_vertical_shader->set_texel_size_mip_index_data(texel_size_mip_index.data());
        glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(bloom_vertical_shader->get_source_texture_index()));
        glBindTexture(GL_TEXTURE_2D, gl_target.get_default().colour_attachments[1]);
        glBindVertexArray(submission::Manager::get().get_screen_vertex_object());
        glDrawArrays(GL_TRIANGLES, 0, 3);
        pop_debug_group();
    }

    GX_GL_CHECK_D;

    for (std::uint32_t layer_index = 0; layer_index < gl_target.get_default().framebuffers[0].size() - 1; ++layer_index) {
        const auto copy_index = (gl_target.get_default().framebuffers[0].size() - 1) - layer_index;
        const auto mip_index = copy_index - 1;

        // we have to do it because of some stupid platforms
        push_debug_group("copy-" + std::to_string(copy_index));
        ctx::set_framebuffer(gl_target.get_default().framebuffers[1][copy_index]);
        ctx::set_viewport_scissor_clip(viewport_clip / (1 << copy_index));
        multiply_shader->bind(current_shader);
        const math::Vec4 value_mip_index(math::Vec3(1.0001f), static_cast<float>(copy_index));
        multiply_shader->set_value_mip_index_data(value_mip_index.data());
        glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(multiply_shader->get_source_texture_index()));
        glBindTexture(GL_TEXTURE_2D, gl_target.get_default().colour_attachments[0]);
        glBindVertexArray(submission::Manager::get().get_screen_vertex_object());
        glDrawArrays(GL_TRIANGLES, 0, 3);
        pop_debug_group();

        push_debug_group("upsampler-" + std::to_string(mip_index));
        ctx::set_framebuffer(gl_target.get_default().framebuffers[0][mip_index]);
        ctx::set_viewport_scissor_clip(viewport_clip / (1 << mip_index));
        bloom_upsampler_shader->bind(current_shader);
        const math::Vec2 scatter_src_mip_index(b.get_scatter_clamp_max_threshold_threshold_knee().x, static_cast<float>(mip_index) + math::Numeric::epsilon<float>);
        bloom_upsampler_shader->set_scatter_src_mip_index_data(scatter_src_mip_index.data());
        glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(bloom_upsampler_shader->get_source_texture_index()));
        glBindTexture(GL_TEXTURE_2D, gl_target.get_default().colour_attachments[1]);
        glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(bloom_upsampler_shader->get_low_texture_index()));
        glBindTexture(GL_TEXTURE_2D, gl_target.get_default().colour_attachments[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        pop_debug_group();
    }

    GX_GL_CHECK_D;

    pop_debug_group();
}

void gearoenix::gl::Camera::render_colour_correction_anti_aliasing(const Scene& scene, const render::record::Camera& rc, uint& current_shader) const
{
    GX_GL_CHECK_D;

    GX_ASSERT_D(target.is_default()); // This function only works on a default-target.

#if GX_GL_LABELING_ENABLED
    static std::string debug_group;
    debug_group.clear();
    debug_group += "render-colour-correction-anti-aliasing for scene: ";
    debug_group += scene.get_object_name();
    debug_group += ", and for camera: ";
    debug_group += object_name;
#endif

    push_debug_group(debug_group);

    glDisable(GL_BLEND);
    const auto texel_size = math::Vec2(1.0f) / (rc.viewport_clip.zw() - rc.viewport_clip.xy());
    const auto viewport_clip = math::Vec4<sizei>(rc.viewport_clip);

    ctx::set_framebuffer(gl_target.get_default().framebuffers[1][0]);
    ctx::set_viewport_scissor_clip(viewport_clip);
    auto& shader = colour_tuning_anti_aliasing_shader_combination->get(colour_tuning);
    shader.bind(current_shader);
    shader.set(colour_tuning);
    shader.set_screen_space_uv_data(texel_size.data());
    glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_source_texture_index()));
    glBindTexture(GL_TEXTURE_2D, gl_target.get_default().colour_attachments[0]);
    glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_depth_texture_index()));
    glBindTexture(GL_TEXTURE_2D, gl_target.get_default().colour_attachments[0]);
    glBindVertexArray(submission::Manager::get().get_screen_vertex_object());
    glDrawArrays(GL_TRIANGLES, 0, 3);
    pop_debug_group();
    GX_GL_CHECK_D;
}

void gearoenix::gl::CameraManager::build(
    std::string&& name,
    core::ecs::Entity* const parent,
    core::job::EndCaller<core::ecs::EntityPtr>&& entity_callback)
{
    build_impl(std::move(name), parent, entity_callback);
    const auto* const entity = entity_callback.get_return().get();
    auto transform = entity->get_component_shared_ptr<physics::Transformation>();
    Camera::construct(
        entity->get_object_name() + "-gl-camera",
        core::job::EndCallerShared<Camera>([end = std::move(entity_callback)](std::shared_ptr<Camera>&& c) {
            end.get_return()->add_component(std::move(c));
        }),
        std::move(transform));
}

gearoenix::gl::CameraManager::CameraManager()
{
    core::ecs::ComponentType::add<Camera>();
}

void gearoenix::gl::CameraManager::window_resized()
{
    Manager::window_resized();
}

gearoenix::gl::CameraManager::~CameraManager() = default;

#endif
