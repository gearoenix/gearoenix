#include "gx-gl-submission-manager.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-world.hpp"
#include "../core/macro/gx-cr-mcr-profiler.hpp"
#include "../physics/animation/gx-phs-anm-armature.hpp"
#include "../physics/animation/gx-phs-anm-manager.hpp"
#include "../physics/gx-phs-transformation.hpp"
#include "../platform/gx-plt-application.hpp"
#include "../render/camera/gx-rnd-cmr-camera.hpp"
#include "../render/material/gx-rnd-mat-material.hpp"
#include "../render/reflection/gx-rnd-rfl-runtime.hpp"
#include "gx-gl-camera.hpp"
#include "gx-gl-check.hpp"
#include "gx-gl-constants.hpp"
#include "gx-gl-context.hpp"
#include "gx-gl-engine.hpp"
#include "gx-gl-label.hpp"
#include "gx-gl-light.hpp"
#include "gx-gl-reflection.hpp"
#include "gx-gl-scene.hpp"
#include "gx-gl-target.hpp"
#include "gx-gl-texture.hpp"
#include "shader/gx-gl-shd-deferred-pbr-transparent.hpp"
#include "shader/gx-gl-shd-deferred-pbr.hpp"
#include "shader/gx-gl-shd-final.hpp"
#include "shader/gx-gl-shd-irradiance.hpp"
#include "shader/gx-gl-shd-manager.hpp"
#include "shader/gx-gl-shd-radiance.hpp"
#include "shader/gx-gl-shd-ssao-resolve.hpp"
#include "shader/gx-gl-shd-unlit.hpp"
#include <ImGui/backends/imgui_impl_opengl3.h>
#include <algorithm>
#include <ranges>

#ifdef GX_PLATFORM_INTERFACE_ANDROID
#include "../../platform/android/gx-plt-gl-context.hpp"
#endif

void gearoenix::gl::submission::Manager::initialise_back_buffer_sizes()
{
    auto& cfg = render::RuntimeConfiguration::get();
    back_buffer_size_changed();
    resolution_cfg_listener_id = cfg.get_runtime_resolution().add_observer([this](const render::Resolution&) -> bool {
        back_buffer_size_changed();
        return true;
    });
}

void gearoenix::gl::submission::Manager::back_buffer_size_changed()
{
    back_buffer_size = math::Vec2(render::texture::Manager::get().get_default_camera_render_target_dimensions());
    back_buffer_aspect_ratio = static_cast<float>(back_buffer_size.x) / static_cast<float>(back_buffer_size.y);
    back_buffer_uv_move = math::Vec2(1.0f) / math::Vec2<float>(back_buffer_size);
    back_buffer_viewport_clip = math::Vec4<sizei>(0, 0, back_buffer_size.x, back_buffer_size.y);

    initialise_gbuffers();
    initialise_ssao();
}

void gearoenix::gl::submission::Manager::initialise_gbuffers()
{
    if (!render::engine::Engine::get().get_specification().is_deferred_supported) {
        return;
    }

    auto& txt_mgr = render::texture::Manager::get();
    const auto position_depth_txt_info = // texture info
        render::texture::TextureInfo()
            .set_format(render::texture::TextureFormat::RgbaFloat32)
            .set_sampler_info(render::texture::SamplerInfo()
                    .set_min_filter(render::texture::Filter::Nearest)
                    .set_mag_filter(render::texture::Filter::Nearest)
                    .set_wrap_s(render::texture::Wrap::ClampToEdge)
                    .set_wrap_t(render::texture::Wrap::ClampToEdge)
                    .set_wrap_r(render::texture::Wrap::ClampToEdge)
                    .set_anisotropic_level(0))
            .set_width(back_buffer_size.x)
            .set_height(back_buffer_size.y)
            .set_depth(0)
            .set_type(render::texture::Type::Texture2D)
            .set_has_mipmap(false);
    txt_mgr.create_2d_from_pixels(
        "gearoenix-opengl-texture-gbuffer-position-depth", {}, position_depth_txt_info,
        core::job::EndCallerShared<render::texture::Texture2D>([this](std::shared_ptr<render::texture::Texture2D>&& t) { gbuffers_position_depth_texture = std::dynamic_pointer_cast<Texture2D>(std::move(t)); }));

    auto albedo_metallic_txt_info = position_depth_txt_info;
    albedo_metallic_txt_info.set_format(render::texture::TextureFormat::RgbaFloat16);
    txt_mgr.create_2d_from_pixels(
        "gearoenix-opengl-texture-gbuffer-albedo-metallic", {}, albedo_metallic_txt_info,
        core::job::EndCallerShared<render::texture::Texture2D>([this](std::shared_ptr<render::texture::Texture2D>&& t) { gbuffers_albedo_metallic_texture = std::dynamic_pointer_cast<Texture2D>(std::move(t)); }));

    auto normal_ao_txt_info = position_depth_txt_info;
    normal_ao_txt_info.set_format(render::texture::TextureFormat::RgbaFloat16);
    txt_mgr.create_2d_from_pixels(
        "gearoenix-opengl-texture-gbuffer-normal-ao", {}, normal_ao_txt_info,
        core::job::EndCallerShared<render::texture::Texture2D>([this](std::shared_ptr<render::texture::Texture2D>&& t) { gbuffers_normal_ao_texture = std::dynamic_pointer_cast<Texture2D>(std::move(t)); }));

    auto emission_roughness_txt_info = position_depth_txt_info;
    emission_roughness_txt_info.set_format(render::texture::TextureFormat::RgbaFloat16);
    txt_mgr.create_2d_from_pixels(
        "gearoenix-opengl-texture-gbuffer-emission-roughness", {}, emission_roughness_txt_info,
        core::job::EndCallerShared<render::texture::Texture2D>([this](std::shared_ptr<render::texture::Texture2D>&& t) { gbuffers_emission_roughness_texture = std::dynamic_pointer_cast<Texture2D>(std::move(t)); }));

    auto irradiance_txt_info = position_depth_txt_info;
    irradiance_txt_info.set_format(render::texture::TextureFormat::RgbFloat16);
    txt_mgr.create_2d_from_pixels(
        "gearoenix-opengl-texture-gbuffer-irradiance", {}, irradiance_txt_info,
        core::job::EndCallerShared<render::texture::Texture2D>([this](std::shared_ptr<render::texture::Texture2D>&& t) { gbuffers_irradiance_texture = std::dynamic_pointer_cast<Texture2D>(std::move(t)); }));

    auto radiance_txt_info = position_depth_txt_info;
    radiance_txt_info.set_format(render::texture::TextureFormat::RgbFloat16);
    txt_mgr.create_2d_from_pixels(
        "gearoenix-opengl-texture-gbuffer-radiance", {}, radiance_txt_info,
        core::job::EndCallerShared<render::texture::Texture2D>([this](std::shared_ptr<render::texture::Texture2D>&& t) { gbuffers_radiance_texture = std::dynamic_pointer_cast<Texture2D>(std::move(t)); }));

    auto depth_txt_info = position_depth_txt_info;
    depth_txt_info.set_format(render::texture::TextureFormat::D32);
    txt_mgr.create_2d_from_pixels(
        "gearoenix-opengl-texture-gbuffer-depth", {}, depth_txt_info,
        core::job::EndCallerShared<render::texture::Texture2D>([this](std::shared_ptr<render::texture::Texture2D>&& t) { gbuffers_depth_texture = std::dynamic_pointer_cast<Texture2D>(std::move(t)); }));

    std::vector<render::texture::Attachment> attachments(GEAROENIX_GL_GBUFFERS_FRAMEBUFFER_ATTACHMENTS_COUNT);
    attachments[GEAROENIX_GL_GBUFFERS_FRAMEBUFFER_ATTACHMENT_INDEX_ALBEDO_METALLIC].var = render::texture::Attachment2D { gbuffers_albedo_metallic_texture };
    attachments[GEAROENIX_GL_GBUFFERS_FRAMEBUFFER_ATTACHMENT_INDEX_POSITION_DEPTH].var = render::texture::Attachment2D { gbuffers_position_depth_texture };
    attachments[GEAROENIX_GL_GBUFFERS_FRAMEBUFFER_ATTACHMENT_INDEX_NORMAL_AO].var = render::texture::Attachment2D { gbuffers_normal_ao_texture };
    attachments[GEAROENIX_GL_GBUFFERS_FRAMEBUFFER_ATTACHMENT_INDEX_EMISSION_ROUGHNESS].var = render::texture::Attachment2D { gbuffers_emission_roughness_texture };
    attachments[GEAROENIX_GL_GBUFFERS_FRAMEBUFFER_ATTACHMENT_INDEX_IRRADIANCE].var = render::texture::Attachment2D { gbuffers_irradiance_texture };
    attachments[GEAROENIX_GL_GBUFFERS_FRAMEBUFFER_ATTACHMENT_INDEX_RADIANCE].var = render::texture::Attachment2D { gbuffers_radiance_texture };
    attachments[GEAROENIX_GL_GBUFFERS_FRAMEBUFFER_ATTACHMENT_INDEX_DEPTH].var = render::texture::Attachment2D { gbuffers_depth_texture };
    txt_mgr.create_target(
        "gearoenix-gbuffers", std::move(attachments),
        core::job::EndCallerShared<render::texture::Target>([this](std::shared_ptr<render::texture::Target>&& t) { gbuffers_target = std::dynamic_pointer_cast<Target>(std::move(t)); }));

    GX_LOG_D("GBuffers have been created.");
}

void gearoenix::gl::submission::Manager::initialise_ssao()
{
    if (!render::engine::Engine::get().get_specification().is_deferred_supported) {
        return;
    }

    auto& txt_mgr = render::texture::Manager::get();
    const auto txt_info = render::texture::TextureInfo()
                              .set_format(render::texture::TextureFormat::Float32)
                              .set_sampler_info(render::texture::SamplerInfo()
                                      .set_min_filter(render::texture::Filter::Nearest)
                                      .set_mag_filter(render::texture::Filter::Nearest)
                                      .set_wrap_s(render::texture::Wrap::ClampToEdge)
                                      .set_wrap_t(render::texture::Wrap::ClampToEdge)
                                      .set_wrap_r(render::texture::Wrap::ClampToEdge)
                                      .set_anisotropic_level(0))
                              .set_width(back_buffer_size.x)
                              .set_height(back_buffer_size.y)
                              .set_depth(0)
                              .set_type(render::texture::Type::Texture2D)
                              .set_has_mipmap(false);
    txt_mgr.create_2d_from_pixels(
        "gearoenix-opengl-texture-ssao-resolve", {}, txt_info,
        core::job::EndCallerShared<render::texture::Texture2D>([this](std::shared_ptr<render::texture::Texture2D>&& t) { ssao_resolve_texture = std::dynamic_pointer_cast<Texture2D>(std::move(t)); }));

    std::vector<render::texture::Attachment> attachments(1);
    attachments[0].var = render::texture::Attachment2D { std::shared_ptr(ssao_resolve_texture) };
    txt_mgr.create_target(
        "gearoenix-ssao", std::move(attachments),
        core::job::EndCallerShared<render::texture::Target>([this](std::shared_ptr<render::texture::Target>&& t) { ssao_resolve_target = std::dynamic_pointer_cast<Target>(std::move(t)); }));

    GX_LOG_D("SSAO resolve buffer has been created.");
}

void gearoenix::gl::submission::Manager::initialise_screen_vertices()
{
    constexpr std::array screen_vertices = {
        -1.0f, 3.0f, // 1
        -1.0f, -1.0f, // 2
        3.0f, -1.0f, // 3
    };
    glGenVertexArrays(1, &screen_vertex_object);
    glBindVertexArray(screen_vertex_object);
    glGenBuffers(1, &screen_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, screen_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screen_vertices), screen_vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, reinterpret_cast<void*>(0));
    glBindVertexArray(0);
}

void gearoenix::gl::submission::Manager::render_shadows()
{
    push_debug_group("render-shadows");
    GX_GL_CHECK_D;
    glDisable(GL_BLEND);
    for (auto* const scene : scenes | std::views::values) {
        scene->render_shadows(current_shader);
    }
    GX_GL_CHECK_D;
    pop_debug_group();
}

void gearoenix::gl::submission::Manager::render_reflection_probes()
{
    push_debug_group("render-reflection-probes");
    core::ecs::World::get().synchronised_system<RuntimeReflection>(
        [&](const auto* const, const auto* const r) {
            constexpr std::array face_uv_axis {
                std::array { math::Vec3(0.0f, 0.0f, -1.0f), math::Vec3(0.0f, -1.0f, 0.0f), math::Vec3(1.0f, 0.0f, 0.0f) }, // PositiveX
                std::array { math::Vec3(0.0f, 0.0f, 1.0f), math::Vec3(0.0f, -1.0f, 0.0f), math::Vec3(-1.0f, 0.0f, 0.0f) }, // NegativeX
                std::array { math::Vec3(1.0f, 0.0f, 0.0f), math::Vec3(0.0f, 0.0f, 1.0f), math::Vec3(0.0f, 1.0f, 0.0f) }, // PositiveY
                std::array { math::Vec3(1.0f, 0.0f, 0.0f), math::Vec3(0.0f, 0.0f, -1.0f), math::Vec3(0.0f, -1.0f, 0.0f) }, // NegativeY
                std::array { math::Vec3(1.0f, 0.0f, 0.0f), math::Vec3(0.0f, -1.0f, 0.0f), math::Vec3(0.0f, 0.0f, 1.0f) }, // PositiveZ
                std::array { math::Vec3(-1.0f, 0.0f, 0.0f), math::Vec3(0.0f, -1.0f, 0.0f), math::Vec3(0.0f, 0.0f, -1.0f) }, // NegativeZ
            };
            switch (r->get_state()) {
            case render::reflection::Runtime::State::EnvironmentCubeMipMap:
                glBindTexture(GL_TEXTURE_CUBE_MAP, r->get_gl_environment_v());
                glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
                return;
            case render::reflection::Runtime::State::IrradianceFace: {
                const auto fi = r->get_state_irradiance_face();
                ctx::set_framebuffer(r->get_gl_irradiance_targets_v()[fi]);
                const auto w = static_cast<sizei>(r->get_irradiance()->get_info().get_width());
                ctx::set_viewport_scissor_clip({ 0, 0, w, w });
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
                irradiance_shader->bind(current_shader);
                glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(irradiance_shader->get_environment_index()));
                glBindTexture(GL_TEXTURE_CUBE_MAP, r->get_gl_environment_v());
                irradiance_shader->set_m_data(reinterpret_cast<const float*>(&face_uv_axis[fi]));
                glBindVertexArray(screen_vertex_object);
                glDrawArrays(GL_TRIANGLES, 0, 3);
                return;
            }
            case render::reflection::Runtime::State::IrradianceMipMap:
                glBindTexture(GL_TEXTURE_CUBE_MAP, r->get_gl_irradiance_v());
                glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
                return;
            case render::reflection::Runtime::State::RadianceFaceLevel: {
                const auto fi = r->get_state_radiance_face();
                const auto li = r->get_state_radiance_level();
                ctx::set_framebuffer(r->get_gl_radiance_targets_v()[fi][li]);
                const auto w = static_cast<sizei>(r->get_radiance()->get_info().get_width() >> li);
                ctx::set_viewport_scissor_clip({ 0, 0, w, w });
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
                radiance_shader->bind(current_shader);
                glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(radiance_shader->get_environment_index()));
                glBindTexture(GL_TEXTURE_CUBE_MAP, r->get_gl_environment_v());
                radiance_shader->set_m_data(reinterpret_cast<const float*>(&face_uv_axis[fi]));
                const auto roughness = static_cast<float>(r->get_roughnesses()[li]);
                radiance_shader->set_roughness_data(&roughness);
                const float roughness_p_4 = roughness * roughness * roughness * roughness;
                radiance_shader->set_roughness_p_4_data(&roughness_p_4);
                const auto resolution = static_cast<float>(r->get_environment()->get_info().get_width());
                const float sa_texel = (static_cast<float>(std::numbers::pi) / 1.5f) / (resolution * resolution);
                radiance_shader->set_sa_texel_data(&sa_texel);
                glBindVertexArray(screen_vertex_object);
                glDrawArrays(GL_TRIANGLES, 0, 3);
                return;
            }
            default:
                return;
            }
        });
    pop_debug_group();
}

void gearoenix::gl::submission::Manager::render_with_deferred()
{
    GX_UNIMPLEMENTED;
    // auto& os_app = e.get_platform_application();
    // const auto& base_os_app = os_app.get_base();
    // const std::array<float, 4> screen_uv_move_reserved { back_buffer_uv_move.x, back_buffer_uv_move.y, 0.0f, 0.0f };
    // const auto* const gbuffers_attachments = gbuffers_target->get_gl_attachments().data();
    // const auto* const ssao_resolved_attachments = ssao_resolve_target->get_gl_attachments().data();
    // for (auto& scene_layer_entity_pool_index : scenes) {
    //     auto& scene = scene_pool[scene_layer_entity_pool_index.second];
    //     glDisable(GL_BLEND); // TODO find the best place for it
    //     set_framebuffer(gbuffers_target->get_framebuffer());
    //     for (auto& camera_layer_entity_pool_index : scene.cameras) {
    //         auto& camera = camera_pool[camera_layer_entity_pool_index.second];
    //         set_viewport_clip(camera.viewport_clip);
    //         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    //         for (auto& distance_model_data : camera.models_data) {
    //             auto& model_data = distance_model_data.second;
    //             model_data.render_deferred_gbuffers(scene, camera, current_shader);
    //         }
    //     }
    //
    //     render_reflection_probes(scene);
    //
    //     // SSAO resolving -------------------------------------------------------------------------------------
    //     set_framebuffer(ssao_resolve_target->get_framebuffer());
    //     set_viewport_clip(back_buffer_viewport_clip);
    //
    //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    //     ssao_resolve_shader->bind(current_shader);
    //
    //     glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(ssao_resolve_shader->get_position_depth_index()));
    //     glBindTexture(GL_TEXTURE_2D, gbuffers_attachments[GEAROENIX_GL_GBUFFERS_FRAMEBUFFER_ATTACHMENT_INDEX_POSITION_DEPTH].texture_object);
    //
    //     glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(ssao_resolve_shader->get_normal_ao_index()));
    //     glBindTexture(GL_TEXTURE_2D, gbuffers_attachments[GEAROENIX_GL_GBUFFERS_FRAMEBUFFER_ATTACHMENT_INDEX_NORMAL_AO].texture_object);
    //
    //     ssao_resolve_shader->set_ssao_radius_move_start_end_data(reinterpret_cast<const float*>(&scene.ssao_settings));
    //     for (auto& camera_layer_entity_pool_index : scene.cameras) {
    //         auto& camera = camera_pool[camera_layer_entity_pool_index.second];
    //         ssao_resolve_shader->set_vp_data(reinterpret_cast<const float*>(&camera.vp));
    //         glBindVertexArray(screen_vertex_object);
    //         glDrawArrays(GL_TRIANGLES, 0, 3);
    //     }
    //     // PBR ------------------------------------------------------------------------------------------------
    //     // set_framebuffer(final_target->get_framebuffer()); TODO camera must do it
    //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    //
    //     for (auto& camera_layer_entity_pool_index : scene.cameras) {
    //         auto& camera = camera_pool[camera_layer_entity_pool_index.second];
    //
    //         render_skyboxes(scene, camera);
    //
    //         deferred_pbr_shader->bind(current_shader);
    //
    //         glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(deferred_pbr_shader->get_position_depth_index()));
    //         glBindTexture(GL_TEXTURE_2D, gbuffers_attachments[GEAROENIX_GL_GBUFFERS_FRAMEBUFFER_ATTACHMENT_INDEX_POSITION_DEPTH].texture_object);
    //
    //         glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(deferred_pbr_shader->get_albedo_metallic_index()));
    //         glBindTexture(GL_TEXTURE_2D, gbuffers_attachments[GEAROENIX_GL_GBUFFERS_FRAMEBUFFER_ATTACHMENT_INDEX_ALBEDO_METALLIC].texture_object);
    //
    //         glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(deferred_pbr_shader->get_normal_ao_index()));
    //         glBindTexture(GL_TEXTURE_2D, gbuffers_attachments[GEAROENIX_GL_GBUFFERS_FRAMEBUFFER_ATTACHMENT_INDEX_NORMAL_AO].texture_object);
    //
    //         glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(deferred_pbr_shader->get_emission_roughness_index()));
    //         glBindTexture(GL_TEXTURE_2D, gbuffers_attachments[GEAROENIX_GL_GBUFFERS_FRAMEBUFFER_ATTACHMENT_INDEX_EMISSION_ROUGHNESS].texture_object);
    //
    //         glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(deferred_pbr_shader->get_irradiance_index()));
    //         glBindTexture(GL_TEXTURE_2D, gbuffers_attachments[GEAROENIX_GL_GBUFFERS_FRAMEBUFFER_ATTACHMENT_INDEX_IRRADIANCE].texture_object);
    //
    //         glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(deferred_pbr_shader->get_radiance_index()));
    //         glBindTexture(GL_TEXTURE_2D, gbuffers_attachments[GEAROENIX_GL_GBUFFERS_FRAMEBUFFER_ATTACHMENT_INDEX_RADIANCE].texture_object);
    //
    //         glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(deferred_pbr_shader->get_ssao_resolved_index()));
    //         glBindTexture(GL_TEXTURE_2D, ssao_resolved_attachments[0].texture_object);
    //
    //         glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(deferred_pbr_shader->get_brdflut_index()));
    //         glBindTexture(GL_TEXTURE_2D, brdflut->get_object());
    //
    //         deferred_pbr_shader->set_screen_uv_move_reserved_data(screen_uv_move_reserved.data());
    //         deferred_pbr_shader->set_camera_position_data(reinterpret_cast<const float*>(&camera.pos));
    //
    //         glBindVertexArray(screen_vertex_object);
    //         glDrawArrays(GL_TRIANGLES, 0, 3);
    //     }
    //
    //     // Final ----------------------------------------------------------------------------------------------
    //     set_framebuffer(0);
    //
    //     const float screen_ratio = static_cast<float>(base_os_app.get_window_size().x) / static_cast<float>(base_os_app.get_window_size().y);
    //     if (screen_ratio < back_buffer_aspect_ratio) {
    //         const auto screen_height = static_cast<sizei>(static_cast<float>(base_os_app.get_window_size().x) / back_buffer_aspect_ratio + 0.1f);
    //         const auto screen_y = (static_cast<sizei>(base_os_app.get_window_size().y) - screen_height) / 2;
    //         set_viewport_clip({ static_cast<sizei>(0), screen_y, static_cast<sizei>(base_os_app.get_window_size().x), screen_height });
    //     } else {
    //         const auto screen_width = static_cast<sizei>(static_cast<float>(base_os_app.get_window_size().y) * back_buffer_aspect_ratio + 0.1f);
    //         const auto screen_x = (static_cast<sizei>(base_os_app.get_window_size().x) - screen_width) / 2;
    //         set_viewport_clip({ screen_x, static_cast<sizei>(0), screen_width, static_cast<sizei>(base_os_app.get_window_size().y) });
    //     }
    //
    //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    //
    //     final_shader->bind(current_shader);
    //
    //     glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(final_shader->get_albedo_index()));
    //     // glBindTexture(GL_TEXTURE_2D, final_attachments[0].texture_object); // TODO camera has the the target
    //
    //     glBindVertexArray(screen_vertex_object);
    //     glDrawArrays(GL_TRIANGLES, 0, 3);
    // }
}

void gearoenix::gl::submission::Manager::render_with_forward()
{
    push_debug_group("render-forward");
    for (const auto& scene : scenes | std::views::values) {
        scene->render_forward(current_shader);
    }
    pop_debug_group();

    const auto& base_os_app = platform::Application::get().get_base();
    const auto& window_size = base_os_app.get_window_size();

    push_debug_group("combine-all-cameras");
    ctx::set_framebuffer(0);
    ctx::set_viewport_scissor_clip({ static_cast<sizei>(0), 0, math::Vec2<sizei>(window_size) });
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    const auto screen_ratio = static_cast<float>(window_size.x) / static_cast<float>(window_size.y);
    if (screen_ratio < back_buffer_aspect_ratio) {
        const auto screen_height = static_cast<sizei>(static_cast<float>(window_size.x) / back_buffer_aspect_ratio + 0.1f);
        const auto screen_y = (static_cast<sizei>(window_size.y) - screen_height) / 2;
        ctx::set_viewport_scissor_clip({ static_cast<sizei>(0), screen_y, static_cast<sizei>(window_size.x), screen_height });
    } else {
        const auto screen_width = static_cast<sizei>(static_cast<float>(window_size.y) * back_buffer_aspect_ratio + 0.1f);
        const auto screen_x = (static_cast<sizei>(window_size.x) - screen_width) / 2;
        ctx::set_viewport_scissor_clip({ screen_x, static_cast<sizei>(0), screen_width, static_cast<sizei>(window_size.y) });
    }

    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    final_shader->bind(current_shader);
    glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(final_shader->get_albedo_index()));
    for (auto* const scene : scenes | std::views::values) {
        const auto& record_cameras = scene->get_record().cameras;
        for (auto& camera_index : record_cameras.mains | std::views::values) {
            auto& camera = *core::cast_ptr<Camera>(record_cameras.cameras[camera_index].camera);
            glBindTexture(GL_TEXTURE_2D, camera.get_gl_target().get_default().colour_attachments[1]);
            glBindVertexArray(screen_vertex_object);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
    }
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    pop_debug_group();
}

void gearoenix::gl::submission::Manager::render_imgui()
{
    push_debug_group("render-imgui");
    ImGui::Render();
    const auto& io = ImGui::GetIO();
    ctx::set_viewport_scissor_clip({ 0, 0, static_cast<sizei>(io.DisplaySize.x), static_cast<sizei>(io.DisplaySize.y) });
    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glGetError();
    pop_debug_group();
}

gearoenix::gl::submission::Manager::Manager()
    : Singleton(this)
    , final_shader(shader::Manager::get().get_shader<shader::Final>())
    , deferred_pbr_shader(render::engine::Engine::get().get_specification().is_deferred_supported ? shader::Manager::get().get_shader<shader::DeferredPbr>() : nullptr)
    , deferred_pbr_transparent_shader(render::engine::Engine::get().get_specification().is_deferred_supported ? shader::Manager::get().get_shader<shader::DeferredPbrTransparent>() : nullptr)
    , irradiance_shader(shader::Manager::get().get_shader<shader::Irradiance>())
    , radiance_shader(shader::Manager::get().get_shader<shader::Radiance>())
    , ssao_resolve_shader(render::engine::Engine::get().get_specification().is_deferred_supported ? shader::Manager::get().get_shader<shader::SsaoResolve>() : nullptr)
    , unlit_shader_combination(shader::Manager::get().get_combiner<shader::UnlitCombination>())
    , unlit_coloured_shader(unlit_shader_combination->get(false, false, true, false))
{
    render::texture::Manager::get().create_cube_from_colour(
        math::Vec4(0.0f),
        core::job::EndCallerShared<render::texture::TextureCube>(
            [this](std::shared_ptr<render::texture::TextureCube>&& t) {
                black_cube = std::dynamic_pointer_cast<TextureCube>(std::move(t));
            }));

    render::texture::Manager::get().get_brdflut(core::job::EndCallerShared<render::texture::Texture2D>(
        [this](std::shared_ptr<render::texture::Texture2D>&& t) {
            brdflut = std::dynamic_pointer_cast<Texture2D>(std::move(t));
        }));

    GX_GL_CHECK_D;
    GX_LOG_D("Creating submission manager.");
    initialise_back_buffer_sizes();
    initialise_screen_vertices();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    // Pipeline settings
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_STENCIL_TEST);

    core::job::execute_current_thread_jobs();
}

gearoenix::gl::submission::Manager::~Manager()
{
    render::RuntimeConfiguration::get().get_runtime_resolution().remove_observer(resolution_cfg_listener_id);
}

void gearoenix::gl::submission::Manager::update()
{
    scenes.clear();
    core::ecs::World::get().synchronised_system<Scene>([&](const auto* const, auto* const scene) -> void {
        if (!scene->get_enabled()) {
            return;
        }
        scenes.emplace(scene->get_layer(), scene);
    });

    render_shadows();
    render_reflection_probes();

    if (render::engine::Engine::get().get_specification().is_deferred_supported) {
        render_with_deferred();
    } else {
        render_with_forward();
    }
    GX_GL_CHECK_D;

    render_imgui();

#ifdef GX_PLATFORM_INTERFACE_SDL2
    SDL_GL_SwapWindow(platform::Application::get().get_window());
#elif defined(GX_PLATFORM_INTERFACE_ANDROID)
    (void)e.get_platform_application().get_gl_context()->swap(); // TODO handle the other states
#endif
}

void gearoenix::gl::submission::Manager::window_resized()
{
    if (render::RuntimeConfiguration::get().get_runtime_resolution().get().get_index() == render::Resolution::fixed_index) {
        return;
    }
    back_buffer_size_changed();
}

#endif
