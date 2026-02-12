#include "gx-vk-cmr-camera.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../descriptor/gx-vk-des-uniform-indexer.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-marker.hpp"
#include "../model/gx-vk-mdl-model.hpp"
#include "../pipeline/gx-vk-pip-push-constant.hpp"
#include "../scene/gx-vk-scn-scene.hpp"
#include "../texture/gx-vk-txt-target.hpp"

#include <ranges>

struct GxShaderDataCameraJointModel;
void gearoenix::vulkan::camera::Camera::set_customised_target(std::shared_ptr<render::texture::Target>&& t)
{
    gapi_target.target = Target::Customised { .target = std::dynamic_pointer_cast<texture::Target>(t) };
    render::camera::Camera::set_customised_target(std::move(t));
}

void gearoenix::vulkan::camera::Camera::update_target(core::job::EndCaller<>&& end)
{
    render::camera::Camera::update_target(core::job::EndCaller([this, self = object_self.lock(), end = std::move(end)] {
        (void)end;
        (void)self;
        gapi_target = Target(target);
    }));
}

gearoenix::vulkan::camera::Camera::Camera(core::ecs::Entity* const entity, const std::string& name, render::camera::Target&& target, std::shared_ptr<physics::Transformation>&& transform)
    : render::camera::Camera(entity, core::ecs::ComponentType::create_index(this), name, std::move(target), std::move(transform))
    , gapi_target(this->target)
{
}

void gearoenix::vulkan::camera::Camera::construct(core::ecs::Entity* const entity, const std::string& name, core::job::EndCallerShared<Camera>&& c, std::shared_ptr<physics::Transformation>&& transform)
{
    c.set_return(Object::construct<Camera>(entity, name, render::camera::Target(), std::move(transform)));
    core::job::send_job(core::Singleton<engine::Engine>::get().get_jobs_thread_id(), [c] {
        auto& s = *c.get_return();
        // self.bloom_prefilter_shader = shader::Manager::get().get_shader<shader::BloomPrefilter>();
        // self.bloom_horizontal_shader = shader::Manager::get().get_shader<shader::BloomHorizontal>();
        // self.bloom_vertical_shader = shader::Manager::get().get_shader<shader::BloomVertical>();
        // self.bloom_upsampler_shader = shader::Manager::get().get_shader<shader::BloomUpsampler>();
        // self.multiply_shader = shader::Manager::get().get_shader<shader::Multiply>();
        // self.skybox_cube_shader = shader::Manager::get().get_shader<shader::SkyboxCube>();
        // self.skybox_equirectangular_shader = shader::Manager::get().get_shader<shader::SkyboxEquirectangular>();
        // self.colour_tuning_anti_aliasing_shader_combination = shader::Manager::get().get_combiner<shader::ColourTuningAntiAliasingCombination>();
    });
    c.get_return()->initialise();
    c.get_return()->update_target(core::job::EndCaller([c] {
        c.get_return()->enable_bloom();
    }));
}

gearoenix::vulkan::camera::Camera::~Camera() = default;

void gearoenix::vulkan::camera::Camera::render_shadow(const render::record::Camera& cmr, const VkCommandBuffer cmd, pipeline::PushConstants& pc, VkPipeline& current_bound_pipeline) const
{
    GX_VK_PUSH_DEBUG_GROUP(cmd, 0.3f, 0.5f, 0.9f, "render-shadow-camera for camera: {}", object_name);

    pc.camera_index = shader_data_index;

    // Shadow's camera must always have a customised target.
    GX_ASSERT_D(target.is_customised());
    const auto render_scope = gapi_target.get_customised().target->create_rendering_scope(cmd);

    record_viewport(cmr, cmd);

    const auto render_models = [&](const auto& models) {
        for (const auto& camera_model : models | std::views::values) {
            GX_ASSERT_D(camera_model.first_mvp_index != static_cast<std::uint32_t>(-1));
            pc.camera_model_index = cameras_joint_model_indices[camera_model.first_mvp_index];
            core::cast_ptr<model::Model>(camera_model.model->model)->render_shadow(camera_model, cmd, pc, current_bound_pipeline);
        }
    };

    render_models(cmr.opaque_models);
    render_models(cmr.translucent_models);
}

void gearoenix::vulkan::camera::Camera::render_forward(
    const render::record::Camera& cmr, const VkCommandBuffer cmd, pipeline::PushConstants& pc, VkPipeline& current_bound_pipeline) const
{
    GX_VK_PUSH_DEBUG_GROUP(cmd, 0.8f, 0.4f, 0.6f, "render-forward-camera for camera: {}", object_name);

    pc.camera_index = shader_data_index;

    const auto render_scope = [&] {
        if (target.is_customised()) {
            return gapi_target.get_customised().target->create_rendering_scope(cmd);
        }
        return gapi_target.get_default().main->create_rendering_scope(cmd);
    }();

    record_viewport(cmr, cmd);

    const auto render_models = [&](const auto& models) {
        for (const auto& camera_model : models | std::views::values) {
            pc.camera_model_index = camera_model.first_mvp_index != static_cast<std::uint32_t>(-1) ? cameras_joint_model_indices[camera_model.first_mvp_index]: 0;
            core::cast_ptr<model::Model>(camera_model.model->model)->render_forward(camera_model, cmd, pc, current_bound_pipeline);
        }
    };

    render_models(cmr.opaque_models);
    // render_forward_skyboxes(cmr, cmd); // TODO
    render_models(cmr.translucent_models);
}

void gearoenix::vulkan::camera::Camera::render_forward_skyboxes(const render::record::Camera& cmr, const VkCommandBuffer cmd) const
{
    GX_VK_PUSH_DEBUG_GROUP(cmd, 0.8f, 0.8f, 0.6f, "render-skyboxes for camera: {}", object_name);
    // glDepthMask(GL_FALSE);
    // // Rendering skyboxes
    // const math::Vec4 camera_pos_scale = { math::Vec3<float>(cmr.transform->get_global_position()), cmr.skybox_scale };
    // bool is_equirectangular_current = true;
    // skybox_equirectangular_shader->bind(current_shader);
    // skybox_equirectangular_shader->set_vp_data(cmr.camera->get_view_projection().data());
    // skybox_equirectangular_shader->set_camera_position_box_scale_data(camera_pos_scale.data());
    // auto skybox_texture_bind_index = static_cast<enumerated>(skybox_equirectangular_shader->get_albedo_index());
    // for (const auto& distance_skybox : scene.get_record().skyboxes.skyboxes) {
    //     const auto& skybox = distance_skybox.second;
    //     const auto is_equirectangular = skybox.equirectangular;
    //     if (is_equirectangular != is_equirectangular_current) {
    //         is_equirectangular_current = is_equirectangular;
    //         if (is_equirectangular) {
    //             skybox_equirectangular_shader->bind(current_shader);
    //             skybox_equirectangular_shader->set_vp_data(cmr.camera->get_view_projection().data());
    //             skybox_equirectangular_shader->set_camera_position_box_scale_data(camera_pos_scale.data());
    //             skybox_texture_bind_index = static_cast<enumerated>(skybox_equirectangular_shader->get_albedo_index());
    //         } else {
    //             skybox_cube_shader->bind(current_shader);
    //             skybox_cube_shader->set_vp_data(cmr.camera->get_view_projection().data());
    //             skybox_cube_shader->set_camera_position_box_scale_data(camera_pos_scale.data());
    //             skybox_texture_bind_index = static_cast<enumerated>(skybox_cube_shader->get_albedo_index());
    //         }
    //     }
    //     glActiveTexture(GL_TEXTURE0 + skybox_texture_bind_index);
    //     const auto& gl_sky = *core::cast_ptr<const Skybox>(skybox.skybox);
    //     glBindTexture(is_equirectangular ? GL_TEXTURE_2D : GL_TEXTURE_CUBE_MAP, gl_sky.get_texture_object());
    //     glBindVertexArray(gl_sky.get_vertex_object());
    //     glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
    // }
    // glDepthMask(GL_TRUE);
}

void gearoenix::vulkan::camera::Camera::render_bloom(const scene::Scene& scene, const render::record::Camera& record_cam, const VkCommandBuffer cmd) const
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

    // push_debug_group(debug_group);

    // GX_GL_CHECK_D;

    // glDisable(GL_BLEND);

    // GX_GL_CHECK_D;

    // const auto viewport_clip = math::Vec4<sizei>(record_cam.viewport_clip);

    // push_debug_group("copy");
    // ctx::set_framebuffer(gl_target.get_default().framebuffers[1][0]);
    // ctx::set_viewport_scissor_clip(viewport_clip);
    // multiply_shader->bind(current_shader);
    // {
    //     const math::Vec4 value_mip_index(math::Vec3(exposure.get_enabled() ? exposure.get_value() : 1.0f), 0.0f);
    //     multiply_shader->set_value_mip_index_data(value_mip_index.data());
    // }
    // glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(multiply_shader->get_source_texture_index()));
    // glBindTexture(GL_TEXTURE_2D, gl_target.get_default().colour_attachments[0]);
    // glBindVertexArray(submission::Manager::get().get_screen_vertex_object());
    // glDrawArrays(GL_TRIANGLES, 0, 3);
    // pop_debug_group();
    //
    // math::Vec3 texel_size_mip_index(math::Vec2(1.0f) / (record_cam.viewport_clip.zw() - record_cam.viewport_clip.xy()), 0.0f);
    //
    // push_debug_group("prefilter");
    // GX_GL_CHECK_D;
    // ctx::set_framebuffer(gl_target.get_default().framebuffers[0][1]);
    // GX_GL_CHECK_D;
    // ctx::set_viewport_scissor_clip(viewport_clip / 2);
    // GX_GL_CHECK_D;
    // bloom_prefilter_shader->bind(current_shader);
    // GX_GL_CHECK_D;
    // bloom_prefilter_shader->set_texel_size_data(texel_size_mip_index.data());
    // GX_GL_CHECK_D;
    // bloom_prefilter_shader->set_scatter_clamp_max_threshold_threshold_knee_data(b.get_scatter_clamp_max_threshold_threshold_knee().data());
    // GX_GL_CHECK_D;
    // glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(bloom_prefilter_shader->get_source_texture_index()));
    // GX_GL_CHECK_D;
    // glBindTexture(GL_TEXTURE_2D, gl_target.get_default().colour_attachments[1]);
    // GX_GL_CHECK_D;
    // glBindVertexArray(submission::Manager::get().get_screen_vertex_object());
    // GX_GL_CHECK_D;
    // glDrawArrays(GL_TRIANGLES, 0, 3);
    // pop_debug_group();
    //
    // GX_GL_CHECK_D;
    //
    // for (int layer_index = 1; layer_index < gl_target.get_default().framebuffers[0].size() - 1; ++layer_index) {
    //     texel_size_mip_index = math::Vec3(texel_size_mip_index.xy() * 2.0f, static_cast<float>(layer_index));
    //
    //     push_debug_group("horizontal-" + std::to_string(layer_index));
    //     ctx::set_framebuffer(gl_target.get_default().framebuffers[1][layer_index]);
    //     ctx::set_viewport_scissor_clip(viewport_clip / (1 << layer_index));
    //     bloom_horizontal_shader->bind(current_shader);
    //     bloom_horizontal_shader->set_texel_size_mip_index_data(texel_size_mip_index.data());
    //     glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(bloom_horizontal_shader->get_source_texture_index()));
    //     glBindTexture(GL_TEXTURE_2D, gl_target.get_default().colour_attachments[0]);
    //     glBindVertexArray(submission::Manager::get().get_screen_vertex_object());
    //     glDrawArrays(GL_TRIANGLES, 0, 3);
    //     pop_debug_group();
    //
    //     const auto next_layer = layer_index + 1;
    //     push_debug_group("vertical-" + std::to_string(layer_index));
    //     ctx::set_framebuffer(gl_target.get_default().framebuffers[0][next_layer]);
    //     ctx::set_viewport_scissor_clip(viewport_clip / (1 << next_layer));
    //     bloom_vertical_shader->bind(current_shader);
    //     bloom_vertical_shader->set_texel_size_mip_index_data(texel_size_mip_index.data());
    //     glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(bloom_vertical_shader->get_source_texture_index()));
    //     glBindTexture(GL_TEXTURE_2D, gl_target.get_default().colour_attachments[1]);
    //     glBindVertexArray(submission::Manager::get().get_screen_vertex_object());
    //     glDrawArrays(GL_TRIANGLES, 0, 3);
    //     pop_debug_group();
    // }
    //
    // GX_GL_CHECK_D;
    //
    // for (std::uint32_t layer_index = 0; layer_index < gl_target.get_default().framebuffers[0].size() - 1; ++layer_index) {
    //     const auto copy_index = (gl_target.get_default().framebuffers[0].size() - 1) - layer_index;
    //     const auto mip_index = copy_index - 1;
    //
    //     // we have to do it because of some stupid platforms
    //     push_debug_group("copy-" + std::to_string(copy_index));
    //     ctx::set_framebuffer(gl_target.get_default().framebuffers[1][copy_index]);
    //     ctx::set_viewport_scissor_clip(viewport_clip / (1 << copy_index));
    //     multiply_shader->bind(current_shader);
    //     const math::Vec4 value_mip_index(math::Vec3(1.0001f), static_cast<float>(copy_index));
    //     multiply_shader->set_value_mip_index_data(value_mip_index.data());
    //     glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(multiply_shader->get_source_texture_index()));
    //     glBindTexture(GL_TEXTURE_2D, gl_target.get_default().colour_attachments[0]);
    //     glBindVertexArray(submission::Manager::get().get_screen_vertex_object());
    //     glDrawArrays(GL_TRIANGLES, 0, 3);
    //     pop_debug_group();
    //
    //     push_debug_group("upsampler-" + std::to_string(mip_index));
    //     ctx::set_framebuffer(gl_target.get_default().framebuffers[0][mip_index]);
    //     ctx::set_viewport_scissor_clip(viewport_clip / (1 << mip_index));
    //     bloom_upsampler_shader->bind(current_shader);
    //     const math::Vec2 scatter_src_mip_index(b.get_scatter_clamp_max_threshold_threshold_knee().x, static_cast<float>(mip_index) + math::Numeric::epsilon<float>);
    //     bloom_upsampler_shader->set_scatter_src_mip_index_data(scatter_src_mip_index.data());
    //     glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(bloom_upsampler_shader->get_source_texture_index()));
    //     glBindTexture(GL_TEXTURE_2D, gl_target.get_default().colour_attachments[1]);
    //     glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(bloom_upsampler_shader->get_low_texture_index()));
    //     glBindTexture(GL_TEXTURE_2D, gl_target.get_default().colour_attachments[1]);
    //     glDrawArrays(GL_TRIANGLES, 0, 3);
    //     pop_debug_group();
    // }
    //
    // GX_GL_CHECK_D;
    //
    // pop_debug_group();
}

void gearoenix::vulkan::camera::Camera::render_colour_correction_anti_aliasing(const scene::Scene& scene, const render::record::Camera& rc, const VkCommandBuffer cmd) const
{
    // GX_GL_CHECK_D;

    GX_ASSERT_D(target.is_default()); // This function only works on a default-target.

#if GX_GL_LABELING_ENABLED
    static std::string debug_group;
    debug_group.clear();
    debug_group += "render-colour-correction-anti-aliasing for scene: ";
    debug_group += scene.get_object_name();
    debug_group += ", and for camera: ";
    debug_group += object_name;
#endif

    // push_debug_group(debug_group);
    //
    // glDisable(GL_BLEND);
    // const auto texel_size = math::Vec2(1.0f) / (rc.viewport_clip.zw() - rc.viewport_clip.xy());
    // const auto viewport_clip = math::Vec4<sizei>(rc.viewport_clip);
    //
    // ctx::set_framebuffer(gl_target.get_default().framebuffers[1][0]);
    // ctx::set_viewport_scissor_clip(viewport_clip);
    // auto& shader = colour_tuning_anti_aliasing_shader_combination->get(colour_tuning);
    // shader.bind(current_shader);
    // shader.set(colour_tuning);
    // shader.set_screen_space_uv_data(texel_size.data());
    // glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_source_texture_index()));
    // glBindTexture(GL_TEXTURE_2D, gl_target.get_default().colour_attachments[0]);
    // glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_depth_texture_index()));
    // glBindTexture(GL_TEXTURE_2D, gl_target.get_default().colour_attachments[0]);
    // glBindVertexArray(submission::Manager::get().get_screen_vertex_object());
    // glDrawArrays(GL_TRIANGLES, 0, 3);
    // pop_debug_group();
    // GX_GL_CHECK_D;
}

void gearoenix::vulkan::camera::Camera::after_record(const std::uint64_t frame_number, const render::record::Camera& rc)
{
    if (frame_number == last_update_frame_number) {
        return;
    }
    last_update_frame_number = frame_number;

    cameras_joint_model_indices.clear();

    {
        auto sd = descriptor::UniformIndexer<GxShaderDataCamera>::get().get_next();
        shader_data_index = sd.get_index();
        auto& [vp, position_reserved] = *sd.get_ptr();
        position_reserved = {math::Vec3<float>(rc.transform->get_local_position()), 1.0f};
        vp = view_projection;
    }

    for (const auto& mvp: rc.mvps) {
        auto sd = descriptor::UniformIndexer<GxShaderDataCameraJointModel>::get().get_next();
        sd.get_ptr()->mvp = mvp;
        cameras_joint_model_indices.push_back(sd.get_index());
    }

    for (const auto& camera_model : rc.opaque_models | std::views::values) {
        core::cast_ptr<model::Model>(camera_model.model->model)->after_record(frame_number, camera_model);
    }

    for (const auto& camera_model : rc.translucent_models | std::views::values) {
        core::cast_ptr<model::Model>(camera_model.model->model)->after_record(frame_number, camera_model);
    }
}

void gearoenix::vulkan::camera::Camera::record_viewport(const render::record::Camera& cmr, const VkCommandBuffer cmd)
{
    const VkViewport viewport {
        .x = cmr.viewport_clip.x,
        .y = cmr.viewport_clip.y,
        .width = cmr.viewport_clip.z,
        .height = cmr.viewport_clip.w,
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
    vkCmdSetViewport(cmd, 0, 1, &viewport);

    const VkRect2D scissor {
        .offset = {
            .x = static_cast<std::int32_t>(cmr.viewport_clip.x),
            .y = static_cast<std::int32_t>(cmr.viewport_clip.y),
        },
        .extent = {
            .width = static_cast<std::uint32_t>(cmr.viewport_clip.z),
            .height = static_cast<std::uint32_t>(cmr.viewport_clip.w),
        },
    };
    vkCmdSetScissor(cmd, 0, 1, &scissor);
}
#endif