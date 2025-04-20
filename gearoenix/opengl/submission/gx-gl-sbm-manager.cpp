#include "gx-gl-sbm-manager.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../core/macro/gx-cr-mcr-profiler.hpp"
#include "../../physics/animation/gx-phs-anm-armature.hpp"
#include "../../physics/animation/gx-phs-anm-manager.hpp"
#include "../../physics/collider/gx-phs-cld-aabb.hpp"
#include "../../physics/collider/gx-phs-cld-frustum.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../../render/camera/gx-rnd-cmr-camera.hpp"
#include "../../render/light/gx-rnd-lt-directional.hpp"
#include "../../render/material/gx-rnd-mat-material.hpp"
#include "../../render/model/gx-rnd-mdl-model.hpp"
#include "../../render/reflection/gx-rnd-rfl-baked.hpp"
#include "../../render/reflection/gx-rnd-rfl-runtime.hpp"
#include "../../render/scene/gx-rnd-scn-scene.hpp"
#include "../../render/skybox/gx-rnd-sky-skybox.hpp"
#include "../gx-gl-camera.hpp"
#include "../gx-gl-check.hpp"
#include "../gx-gl-constants.hpp"
#include "../gx-gl-engine.hpp"
#include "../gx-gl-label.hpp"
#include "../gx-gl-light.hpp"
#include "../gx-gl-mesh.hpp"
#include "../gx-gl-model.hpp"
#include "../gx-gl-reflection.hpp"
#include "../gx-gl-skybox.hpp"
#include "../gx-gl-target.hpp"
#include "../gx-gl-texture.hpp"
#include "../material/gx-gl-material.hpp"
#include "../shader/gx-gl-shd-bloom.hpp"
#include "../shader/gx-gl-shd-ctaa.hpp"
#include "../shader/gx-gl-shd-deferred-pbr-transparent.hpp"
#include "../shader/gx-gl-shd-deferred-pbr.hpp"
#include "../shader/gx-gl-shd-final.hpp"
#include "../shader/gx-gl-shd-irradiance.hpp"
#include "../shader/gx-gl-shd-manager.hpp"
#include "../shader/gx-gl-shd-multiply.hpp"
#include "../shader/gx-gl-shd-radiance.hpp"
#include "../shader/gx-gl-shd-skybox-cube.hpp"
#include "../shader/gx-gl-shd-skybox-equirectangular.hpp"
#include "../shader/gx-gl-shd-ssao-resolve.hpp"
#include "../shader/gx-gl-shd-unlit.hpp"
#include <algorithm>
#include <boost/mp11/algorithm.hpp>
#include <imgui/backends/imgui_impl_opengl3.h>



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
    back_buffer_size = math::Vec2(e.get_texture_manager()->get_default_camera_render_target_dimensions());
    back_buffer_aspect_ratio = static_cast<float>(back_buffer_size.x) / static_cast<float>(back_buffer_size.y);
    back_buffer_uv_move = math::Vec2(1.0f) / math::Vec2<float>(back_buffer_size);
    back_buffer_viewport_clip = math::Vec4<sizei>(0, 0, back_buffer_size.x, back_buffer_size.y);

    initialise_gbuffers();
    initialise_ssao();
}

void gearoenix::gl::submission::Manager::initialise_gbuffers()
{
    if (!e.get_specification().is_deferred_supported)
        return;
    auto* const txt_mgr = e.get_texture_manager();
    const auto position_depth_txt_info = render::texture::TextureInfo()
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
    txt_mgr->create_2d_from_pixels(
        "gearoenix-opengl-texture-gbuffer-position-depth", {}, position_depth_txt_info,
        core::job::EndCallerShared<render::texture::Texture2D>([this](std::shared_ptr<render::texture::Texture2D>&& t) { gbuffers_position_depth_texture = std::dynamic_pointer_cast<Texture2D>(std::move(t)); }));

    auto albedo_metallic_txt_info = position_depth_txt_info;
    albedo_metallic_txt_info.set_format(render::texture::TextureFormat::RgbaFloat16);
    txt_mgr->create_2d_from_pixels(
        "gearoenix-opengl-texture-gbuffer-albedo-metallic", {}, albedo_metallic_txt_info,
        core::job::EndCallerShared<render::texture::Texture2D>([this](std::shared_ptr<render::texture::Texture2D>&& t) { gbuffers_albedo_metallic_texture = std::dynamic_pointer_cast<Texture2D>(std::move(t)); }));

    auto normal_ao_txt_info = position_depth_txt_info;
    normal_ao_txt_info.set_format(render::texture::TextureFormat::RgbaFloat16);
    txt_mgr->create_2d_from_pixels(
        "gearoenix-opengl-texture-gbuffer-normal-ao", {}, normal_ao_txt_info,
        core::job::EndCallerShared<render::texture::Texture2D>([this](std::shared_ptr<render::texture::Texture2D>&& t) { gbuffers_normal_ao_texture = std::dynamic_pointer_cast<Texture2D>(std::move(t)); }));

    auto emission_roughness_txt_info = position_depth_txt_info;
    emission_roughness_txt_info.set_format(render::texture::TextureFormat::RgbaFloat16);
    txt_mgr->create_2d_from_pixels(
        "gearoenix-opengl-texture-gbuffer-emission-roughness", {}, emission_roughness_txt_info,
        core::job::EndCallerShared<render::texture::Texture2D>([this](std::shared_ptr<render::texture::Texture2D>&& t) { gbuffers_emission_roughness_texture = std::dynamic_pointer_cast<Texture2D>(std::move(t)); }));

    auto irradiance_txt_info = position_depth_txt_info;
    irradiance_txt_info.set_format(render::texture::TextureFormat::RgbFloat16);
    txt_mgr->create_2d_from_pixels(
        "gearoenix-opengl-texture-gbuffer-irradiance", {}, irradiance_txt_info,
        core::job::EndCallerShared<render::texture::Texture2D>([this](std::shared_ptr<render::texture::Texture2D>&& t) { gbuffers_irradiance_texture = std::dynamic_pointer_cast<Texture2D>(std::move(t)); }));

    auto radiance_txt_info = position_depth_txt_info;
    radiance_txt_info.set_format(render::texture::TextureFormat::RgbFloat16);
    txt_mgr->create_2d_from_pixels(
        "gearoenix-opengl-texture-gbuffer-radiance", {}, radiance_txt_info,
        core::job::EndCallerShared<render::texture::Texture2D>([this](std::shared_ptr<render::texture::Texture2D>&& t) { gbuffers_radiance_texture = std::dynamic_pointer_cast<Texture2D>(std::move(t)); }));

    auto depth_txt_info = position_depth_txt_info;
    depth_txt_info.set_format(render::texture::TextureFormat::D32);
    txt_mgr->create_2d_from_pixels(
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
    e.get_texture_manager()->create_target(
        "gearoenix-gbuffers", std::move(attachments),
        core::job::EndCallerShared<render::texture::Target>([this](std::shared_ptr<render::texture::Target>&& t) { gbuffers_target = std::dynamic_pointer_cast<Target>(std::move(t)); }));

    GX_LOG_D("GBuffers have been created.");
}

void gearoenix::gl::submission::Manager::initialise_ssao()
{
    if (!e.get_specification().is_deferred_supported)
        return;
    auto* const txt_mgr = e.get_texture_manager();
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
    txt_mgr->create_2d_from_pixels(
        "gearoenix-opengl-texture-ssao-resolve", {}, txt_info,
        core::job::EndCallerShared<render::texture::Texture2D>([this](std::shared_ptr<render::texture::Texture2D>&& t) { ssao_resolve_texture = std::dynamic_pointer_cast<Texture2D>(std::move(t)); }));

    std::vector<render::texture::Attachment> attachments(1);
    attachments[0].var = render::texture::Attachment2D { std::shared_ptr(ssao_resolve_texture) };
    e.get_texture_manager()->create_target(
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

void gearoenix::gl::submission::Manager::fill_scenes()
{
    core::ecs::World::get().synchronised_system<render::scene::Scene>([&, this](auto* const scene_entity, const auto* const scene) {
        if (!scene->get_enabled()) {
            return;
        }
        const auto scene_pool_index = scene_pool.emplace([] { return Scene(); });
        auto& scene_pool_ref = scene_pool[scene_pool_index];

        scenes.emplace(std::make_pair(scene->get_layer(), scene_entity), scene_pool_index);
    });
}

void gearoenix::gl::submission::Manager::update_scenes()
{
    core::ecs::World::get().parallel_system<render::scene::Scene>(
        [&, this](auto* const scene_entity, auto* const render_scene, const auto /*kernel_index*/) {
            if (!render_scene->get_enabled()) {
                return;
            }
            auto& scene_data = scene_pool[scenes[std::make_pair(render_scene->get_layer(), scene_entity)]];
            update_scene(scene_entity, scene_data, *render_scene);
        });
}

void gearoenix::gl::submission::Manager::render_shadows()
{
    push_debug_group("render-shadows");
    GX_GL_CHECK_D;
    glDisable(GL_BLEND);
    for (auto& scene_layer_entity_pool_index : scenes) {
        auto& scene = scene_pool[scene_layer_entity_pool_index.second];
        render_shadows(scene);
    }
    GX_GL_CHECK_D;
    pop_debug_group();
}

void gearoenix::gl::submission::Manager::render_shadows(const Scene& scene, const Camera& camera)
{
    push_debug_group("render-shadows for camera " + *camera.name);
    set_framebuffer(camera.main_framebuffer);
    set_viewport_clip(camera.viewport_clip);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    for (auto& distance_model_data : camera.models_data) {
        auto& model_data = distance_model_data.second;
        model_data.render_shadow(scene, camera, current_shader);
    }
    pop_debug_group();
}

void gearoenix::gl::submission::Manager::render_shadows(const Scene& scene)
{
    push_debug_group("render-shadows for scene " + *scene.name);
    for (auto& camera_pool_index : scene.shadow_cameras) {
        auto& camera = camera_pool[camera_pool_index.second];
        render_shadows(scene, camera);
    }
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
                set_framebuffer(r->get_gl_irradiance_targets_v()[fi]);
                const auto w = static_cast<sizei>(r->get_irradiance()->get_info().get_width());
                set_viewport_clip({ 0, 0, w, w });
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
                set_framebuffer(r->get_gl_radiance_targets_v()[fi][li]);
                const auto w = static_cast<sizei>(r->get_radiance()->get_info().get_width() >> li);
                set_viewport_clip({ 0, 0, w, w });
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
                radiance_shader->bind(current_shader);
                glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(radiance_shader->get_environment_index()));
                glBindTexture(GL_TEXTURE_CUBE_MAP, r->get_gl_environment_v());
                radiance_shader->set_m_data(reinterpret_cast<const float*>(&face_uv_axis[fi]));
                const auto roughness = static_cast<float>(r->get_roughnesses()[li]);
                radiance_shader->set_roughness_data(reinterpret_cast<const float*>(&roughness));
                const float roughness_p_4 = roughness * roughness * roughness * roughness;
                radiance_shader->set_roughness_p_4_data(reinterpret_cast<const float*>(&roughness_p_4));
                const auto resolution = static_cast<float>(r->get_environment()->get_info().get_width());
                const float sa_texel = (static_cast<float>(std::numbers::pi) / 1.5f) / (resolution * resolution);
                radiance_shader->set_sa_texel_data(reinterpret_cast<const float*>(&sa_texel));
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

void gearoenix::gl::submission::Manager::render_reflection_probes(const Scene& scene)
{
    push_debug_group("render-reflection-probes for scene " + *scene.name);
    for (auto& camera_pool_index : scene.reflection_cameras) {
        auto& camera = camera_pool[camera_pool_index.second];
        render_forward_camera(scene, camera);
    }
    pop_debug_group();
}

void gearoenix::gl::submission::Manager::render_skyboxes(const Scene& scene, const Camera& camera)
{
    push_debug_group("render-skyboxes for scene " + *scene.name + " and for camera " + *camera.name);
    glDepthMask(GL_FALSE);
    // Rendering skyboxes
    const auto camera_pos_scale = math::Vec4(camera.pos, camera.skybox_scale);
    bool is_equirectangular_current = true;
    skybox_equirectangular_shader->bind(current_shader);
    skybox_equirectangular_shader->set_vp_data(reinterpret_cast<const float*>(&camera.vp));
    skybox_equirectangular_shader->set_camera_position_box_scale_data(reinterpret_cast<const float*>(&camera_pos_scale));
    auto skybox_texture_bind_index = static_cast<enumerated>(skybox_equirectangular_shader->get_albedo_index());
    for (const auto& key_skybox : scene.skyboxes) {
        const auto is_equirectangular = std::get<2>(key_skybox.first);
        if (is_equirectangular != is_equirectangular_current) {
            is_equirectangular_current = is_equirectangular;
            if (is_equirectangular) {
                skybox_equirectangular_shader->bind(current_shader);
                skybox_equirectangular_shader->set_vp_data(reinterpret_cast<const float*>(&camera.vp));
                skybox_equirectangular_shader->set_camera_position_box_scale_data(reinterpret_cast<const float*>(&camera_pos_scale));
                skybox_texture_bind_index = static_cast<enumerated>(skybox_equirectangular_shader->get_albedo_index());
            } else {
                skybox_cube_shader->bind(current_shader);
                skybox_cube_shader->set_vp_data(reinterpret_cast<const float*>(&camera.vp));
                skybox_cube_shader->set_camera_position_box_scale_data(reinterpret_cast<const float*>(&camera_pos_scale));
                skybox_texture_bind_index = static_cast<enumerated>(skybox_cube_shader->get_albedo_index());
            }
        }
        const auto& skybox = key_skybox.second;
        glActiveTexture(GL_TEXTURE0 + skybox_texture_bind_index);
        glBindTexture(is_equirectangular ? GL_TEXTURE_2D : GL_TEXTURE_CUBE_MAP, skybox.albedo_txt);
        glBindVertexArray(skybox.vertex_object);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
    }
    glDepthMask(GL_TRUE);
    pop_debug_group();
}

void gearoenix::gl::submission::Manager::render_forward_camera(const Scene& scene, const Camera& camera)
{
    push_debug_group("render-forward-camera for scene " + *scene.name + " and for camera " + *camera.name);
    GX_GL_CHECK_D;
    set_framebuffer(camera.main_framebuffer);
    set_viewport_clip(camera.viewport_clip);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    render_skyboxes(scene, camera);
    GX_GL_CHECK_D;
    glEnable(GL_BLEND); /// TODO: take it into a context and material must decide
    // Rendering forward pbr
    for (auto& distance_model_data : camera.models_data) {
        auto& model_data = distance_model_data.second;
        model_data.render_forward(scene, camera, current_shader);
        GX_GL_CHECK_D;
    }
    glDisable(GL_BLEND); /// TODO: take it into a context and material must decide
    pop_debug_group();
}

void gearoenix::gl::submission::Manager::render_with_deferred()
{
    auto& os_app = e.get_platform_application();
    const auto& base_os_app = os_app.get_base();
    const std::array<float, 4> screen_uv_move_reserved { back_buffer_uv_move.x, back_buffer_uv_move.y, 0.0f, 0.0f };
    const auto* const gbuffers_attachments = gbuffers_target->get_gl_attachments().data();
    const auto* const ssao_resolved_attachments = ssao_resolve_target->get_gl_attachments().data();
    for (auto& scene_layer_entity_pool_index : scenes) {
        auto& scene = scene_pool[scene_layer_entity_pool_index.second];
        glDisable(GL_BLEND); // TODO find the best place for it
        set_framebuffer(gbuffers_target->get_framebuffer());
        for (auto& camera_layer_entity_pool_index : scene.cameras) {
            auto& camera = camera_pool[camera_layer_entity_pool_index.second];
            set_viewport_clip(camera.viewport_clip);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            for (auto& distance_model_data : camera.models_data) {
                auto& model_data = distance_model_data.second;
                model_data.render_deferred_gbuffers(scene, camera, current_shader);
            }
        }

        render_reflection_probes(scene);

        // SSAO resolving -------------------------------------------------------------------------------------
        set_framebuffer(ssao_resolve_target->get_framebuffer());
        set_viewport_clip(back_buffer_viewport_clip);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        ssao_resolve_shader->bind(current_shader);

        glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(ssao_resolve_shader->get_position_depth_index()));
        glBindTexture(GL_TEXTURE_2D, gbuffers_attachments[GEAROENIX_GL_GBUFFERS_FRAMEBUFFER_ATTACHMENT_INDEX_POSITION_DEPTH].texture_object);

        glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(ssao_resolve_shader->get_normal_ao_index()));
        glBindTexture(GL_TEXTURE_2D, gbuffers_attachments[GEAROENIX_GL_GBUFFERS_FRAMEBUFFER_ATTACHMENT_INDEX_NORMAL_AO].texture_object);

        ssao_resolve_shader->set_ssao_radius_move_start_end_data(reinterpret_cast<const float*>(&scene.ssao_settings));
        for (auto& camera_layer_entity_pool_index : scene.cameras) {
            auto& camera = camera_pool[camera_layer_entity_pool_index.second];
            ssao_resolve_shader->set_vp_data(reinterpret_cast<const float*>(&camera.vp));
            glBindVertexArray(screen_vertex_object);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
        // PBR ------------------------------------------------------------------------------------------------
        // set_framebuffer(final_target->get_framebuffer()); TODO camera must do it
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        for (auto& camera_layer_entity_pool_index : scene.cameras) {
            auto& camera = camera_pool[camera_layer_entity_pool_index.second];

            render_skyboxes(scene, camera);

            deferred_pbr_shader->bind(current_shader);

            glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(deferred_pbr_shader->get_position_depth_index()));
            glBindTexture(GL_TEXTURE_2D, gbuffers_attachments[GEAROENIX_GL_GBUFFERS_FRAMEBUFFER_ATTACHMENT_INDEX_POSITION_DEPTH].texture_object);

            glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(deferred_pbr_shader->get_albedo_metallic_index()));
            glBindTexture(GL_TEXTURE_2D, gbuffers_attachments[GEAROENIX_GL_GBUFFERS_FRAMEBUFFER_ATTACHMENT_INDEX_ALBEDO_METALLIC].texture_object);

            glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(deferred_pbr_shader->get_normal_ao_index()));
            glBindTexture(GL_TEXTURE_2D, gbuffers_attachments[GEAROENIX_GL_GBUFFERS_FRAMEBUFFER_ATTACHMENT_INDEX_NORMAL_AO].texture_object);

            glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(deferred_pbr_shader->get_emission_roughness_index()));
            glBindTexture(GL_TEXTURE_2D, gbuffers_attachments[GEAROENIX_GL_GBUFFERS_FRAMEBUFFER_ATTACHMENT_INDEX_EMISSION_ROUGHNESS].texture_object);

            glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(deferred_pbr_shader->get_irradiance_index()));
            glBindTexture(GL_TEXTURE_2D, gbuffers_attachments[GEAROENIX_GL_GBUFFERS_FRAMEBUFFER_ATTACHMENT_INDEX_IRRADIANCE].texture_object);

            glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(deferred_pbr_shader->get_radiance_index()));
            glBindTexture(GL_TEXTURE_2D, gbuffers_attachments[GEAROENIX_GL_GBUFFERS_FRAMEBUFFER_ATTACHMENT_INDEX_RADIANCE].texture_object);

            glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(deferred_pbr_shader->get_ssao_resolved_index()));
            glBindTexture(GL_TEXTURE_2D, ssao_resolved_attachments[0].texture_object);

            glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(deferred_pbr_shader->get_brdflut_index()));
            glBindTexture(GL_TEXTURE_2D, brdflut->get_object());

            deferred_pbr_shader->set_screen_uv_move_reserved_data(screen_uv_move_reserved.data());
            deferred_pbr_shader->set_camera_position_data(reinterpret_cast<const float*>(&camera.pos));

            glBindVertexArray(screen_vertex_object);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        // Final ----------------------------------------------------------------------------------------------
        set_framebuffer(0);

        const float screen_ratio = static_cast<float>(base_os_app.get_window_size().x) / static_cast<float>(base_os_app.get_window_size().y);
        if (screen_ratio < back_buffer_aspect_ratio) {
            const auto screen_height = static_cast<sizei>(static_cast<float>(base_os_app.get_window_size().x) / back_buffer_aspect_ratio + 0.1f);
            const auto screen_y = (static_cast<sizei>(base_os_app.get_window_size().y) - screen_height) / 2;
            set_viewport_clip({ static_cast<sizei>(0), screen_y, static_cast<sizei>(base_os_app.get_window_size().x), screen_height });
        } else {
            const auto screen_width = static_cast<sizei>(static_cast<float>(base_os_app.get_window_size().y) * back_buffer_aspect_ratio + 0.1f);
            const auto screen_x = (static_cast<sizei>(base_os_app.get_window_size().x) - screen_width) / 2;
            set_viewport_clip({ screen_x, static_cast<sizei>(0), screen_width, static_cast<sizei>(base_os_app.get_window_size().y) });
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        final_shader->bind(current_shader);

        glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(final_shader->get_albedo_index()));
        // glBindTexture(GL_TEXTURE_2D, final_attachments[0].texture_object); // TODO camera has the the target

        glBindVertexArray(screen_vertex_object);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
}

void gearoenix::gl::submission::Manager::render_with_forward()
{
    push_debug_group("render-forward");
    for (auto& scene_layer_entity_pool_index : scenes) {
        auto& scene = scene_pool[scene_layer_entity_pool_index.second];
        render_reflection_probes(scene);
        for (auto& camera_layer_entity_pool_index : scene.cameras) {
            auto& camera = camera_pool[camera_layer_entity_pool_index.second];
            render_forward_camera(scene, camera);
            render_bloom(scene, camera);
            render_colour_correction_anti_aliasing(scene, camera);
        }
        render_debug_meshes(scene);
    }
    pop_debug_group();

    push_debug_group("combine-all-cameras");
    set_framebuffer(0);
    auto& base_os_app = e.get_platform_application().get_base();
    const auto& window_size = base_os_app.get_window_size();
    set_viewport_clip({ static_cast<sizei>(0), static_cast<sizei>(0), static_cast<sizei>(window_size.x), static_cast<sizei>(window_size.y) });
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    const float screen_ratio = static_cast<float>(window_size.x) / static_cast<float>(window_size.y);
    if (screen_ratio < back_buffer_aspect_ratio) {
        const auto screen_height = static_cast<sizei>(static_cast<float>(window_size.x) / back_buffer_aspect_ratio + 0.1f);
        const auto screen_y = (static_cast<sizei>(window_size.y) - screen_height) / 2;
        set_viewport_clip({ static_cast<sizei>(0), screen_y, static_cast<sizei>(window_size.x), screen_height });
    } else {
        const auto screen_width = static_cast<sizei>(static_cast<float>(window_size.y) * back_buffer_aspect_ratio + 0.1f);
        const auto screen_x = (static_cast<sizei>(window_size.x) - screen_width) / 2;
        set_viewport_clip({ screen_x, static_cast<sizei>(0), screen_width, static_cast<sizei>(window_size.y) });
    }

    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    final_shader->bind(current_shader);
    glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(final_shader->get_albedo_index()));
    for (auto& scene_layer_entity_pool_index : scenes) {
        auto& scene = scene_pool[scene_layer_entity_pool_index.second];
        for (auto& camera_layer_entity_pool_index : scene.cameras) {
            auto& camera = camera_pool[camera_layer_entity_pool_index.second];
            glBindTexture(GL_TEXTURE_2D, camera.colour_attachments[1]);
            glBindVertexArray(screen_vertex_object);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
    }
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    pop_debug_group();
}

void gearoenix::gl::submission::Manager::render_bloom(const Scene& s, const Camera& camera)
{
    if (!camera.bloom_data.has_value()) {
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

    const auto& b = *camera.bloom_data;

    push_debug_group("render-bloom in scene " + *s.name + " for camera " + *camera.name);

    GX_GL_CHECK_D;

    glDisable(GL_BLEND);

    GX_GL_CHECK_D;

    push_debug_group("copy");
    set_framebuffer(camera.framebuffers[1][0]);
    set_viewport_clip(camera.viewport_clip);
    multiply_shader->bind(current_shader);
    {
        const math::Vec4<float> value_mip_index(math::Vec3<float>(camera.exposure_value), 0.0f);
        multiply_shader->set_value_mip_index_data(value_mip_index.data());
    }
    glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(multiply_shader->get_source_texture_index()));
    glBindTexture(GL_TEXTURE_2D, camera.colour_attachments[0]);
    glBindVertexArray(screen_vertex_object);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    pop_debug_group();

    math::Vec3<float> texel_size_mip_index(math::Vec2<float>(1.0f) / math::Vec2<float>(camera.viewport_clip.zw() - camera.viewport_clip.xy()), 0.0f);

    push_debug_group("prefilter");
    GX_GL_CHECK_D;
    set_framebuffer(camera.framebuffers[0][1]);
    GX_GL_CHECK_D;
    set_viewport_clip(camera.viewport_clip / 2);
    GX_GL_CHECK_D;
    bloom_prefilter_shader->bind(current_shader);
    GX_GL_CHECK_D;
    bloom_prefilter_shader->set_texel_size_data(texel_size_mip_index.data());
    GX_GL_CHECK_D;
    bloom_prefilter_shader->set_scatter_clamp_max_threshold_threshold_knee_data(b.scatter_clamp_max_threshold_threshold_knee.data());
    GX_GL_CHECK_D;
    glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(bloom_prefilter_shader->get_source_texture_index()));
    GX_GL_CHECK_D;
    glBindTexture(GL_TEXTURE_2D, camera.colour_attachments[1]);
    GX_GL_CHECK_D;
    glBindVertexArray(screen_vertex_object);
    GX_GL_CHECK_D;
    glDrawArrays(GL_TRIANGLES, 0, 3);
    pop_debug_group();

    GX_GL_CHECK_D;

    for (int layer_index = 1; layer_index < camera.framebuffers[0].size() - 1; ++layer_index) {
        texel_size_mip_index = math::Vec3<float>(texel_size_mip_index.xy() * 2.0f, static_cast<float>(layer_index));

        push_debug_group("horizontal-" + std::to_string(layer_index));
        set_framebuffer(camera.framebuffers[1][layer_index]);
        set_viewport_clip(camera.viewport_clip / (1 << layer_index));
        bloom_horizontal_shader->bind(current_shader);
        bloom_horizontal_shader->set_texel_size_mip_index_data(texel_size_mip_index.data());
        glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(bloom_horizontal_shader->get_source_texture_index()));
        glBindTexture(GL_TEXTURE_2D, camera.colour_attachments[0]);
        glBindVertexArray(screen_vertex_object);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        pop_debug_group();

        const auto next_layer = layer_index + 1;
        push_debug_group("vertical-" + std::to_string(layer_index));
        set_framebuffer(camera.framebuffers[0][next_layer]);
        set_viewport_clip(camera.viewport_clip / (1 << next_layer));
        bloom_vertical_shader->bind(current_shader);
        bloom_vertical_shader->set_texel_size_mip_index_data(texel_size_mip_index.data());
        glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(bloom_vertical_shader->get_source_texture_index()));
        glBindTexture(GL_TEXTURE_2D, camera.colour_attachments[1]);
        glBindVertexArray(screen_vertex_object);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        pop_debug_group();
    }

    GX_GL_CHECK_D;

    for (std::uint32_t layer_index = 0; layer_index < camera.framebuffers[0].size() - 1; ++layer_index) {
        const auto copy_index = (camera.framebuffers[0].size() - 1) - layer_index;
        const auto mip_index = copy_index - 1;

        // we have to do it because of some stupid platforms
        push_debug_group("copy-" + std::to_string(copy_index));
        set_framebuffer(camera.framebuffers[1][copy_index]);
        set_viewport_clip(camera.viewport_clip / (1 << copy_index));
        multiply_shader->bind(current_shader);
        const math::Vec4<float> value_mip_index(math::Vec3<float>(1.0001f), static_cast<float>(copy_index));
        multiply_shader->set_value_mip_index_data(value_mip_index.data());
        glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(multiply_shader->get_source_texture_index()));
        glBindTexture(GL_TEXTURE_2D, camera.colour_attachments[0]);
        glBindVertexArray(screen_vertex_object);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        pop_debug_group();

        push_debug_group("upsampler-" + std::to_string(mip_index));
        set_framebuffer(camera.framebuffers[0][mip_index]);
        set_viewport_clip(camera.viewport_clip / (1 << mip_index));
        bloom_upsampler_shader->bind(current_shader);
        const math::Vec2<float> scatter_src_mip_index(b.scatter_clamp_max_threshold_threshold_knee.x, static_cast<float>(mip_index) + math::Numeric::epsilon<float>);
        bloom_upsampler_shader->set_scatter_src_mip_index_data(scatter_src_mip_index.data());
        glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(bloom_upsampler_shader->get_source_texture_index()));
        glBindTexture(GL_TEXTURE_2D, camera.colour_attachments[1]);
        glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(bloom_upsampler_shader->get_low_texture_index()));
        glBindTexture(GL_TEXTURE_2D, camera.colour_attachments[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        pop_debug_group();
    }

    GX_GL_CHECK_D;

    pop_debug_group();
}

void gearoenix::gl::submission::Manager::render_colour_correction_anti_aliasing(const Scene& s, const Camera& camera)
{
    GX_GL_CHECK_D;
    push_debug_group("render-colour-correction-anti-aliasing in scene " + *s.name + " for camera " + *camera.name);
    glDisable(GL_BLEND);
    const auto texel_size = math::Vec2<float>(1.0f) / math::Vec2<float>(camera.viewport_clip.zw() - camera.viewport_clip.xy());
    set_framebuffer(camera.framebuffers[1][0]);
    set_viewport_clip(camera.viewport_clip);
    auto& shader = colour_tuning_anti_aliasing_shader_combination->get(camera.colour_tuning);
    shader.bind(current_shader);
    shader.set(camera.colour_tuning);
    shader.set_screen_space_uv_data(texel_size.data());
    glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_source_texture_index()));
    glBindTexture(GL_TEXTURE_2D, camera.colour_attachments[0]);
    glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(shader.get_depth_texture_index()));
    glBindTexture(GL_TEXTURE_2D, camera.colour_attachments[0]);
    glBindVertexArray(screen_vertex_object);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    pop_debug_group();
    GX_GL_CHECK_D;
}

void gearoenix::gl::submission::Manager::render_debug_meshes(const Scene& scene)
{
    push_debug_group("render-debug-meshes in scene " + *scene.name);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    unlit_coloured_shader.bind(current_shader);
    for (auto& camera_layer_entity_pool_index : scene.cameras) {
        const auto& camera = camera_pool[camera_layer_entity_pool_index.second];
        for (const auto& mesh : camera.debug_meshes) {
            unlit_coloured_shader.set_mvp_data(reinterpret_cast<const float*>(&mesh.m));
            unlit_coloured_shader.set_albedo_factor_data(reinterpret_cast<const float*>(&mesh.colour));
            glBindVertexArray(mesh.vertex_object);
            glDrawElements(GL_LINES, mesh.indices_count, GL_UNSIGNED_INT, nullptr);
        }
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    pop_debug_group();
}

void gearoenix::gl::submission::Manager::render_imgui()
{
    push_debug_group("render-imgui");
    ImGui::Render();
    const ImGuiIO& io = ImGui::GetIO();
    set_viewport_clip({ 0, 0, static_cast<sizei>(io.DisplaySize.x), static_cast<sizei>(io.DisplaySize.y) });
    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glGetError();
    pop_debug_group();
}

void gearoenix::gl::submission::Manager::set_viewport_clip(const math::Vec4<sizei>& viewport_clip)
{
    if (current_viewport_clip == viewport_clip) {
        return;
    }
    current_viewport_clip = viewport_clip;
    glViewport(
        static_cast<sint>(current_viewport_clip.x), static_cast<sint>(current_viewport_clip.y),
        current_viewport_clip.z, current_viewport_clip.w);
    glScissor(
        static_cast<sint>(current_viewport_clip.x), static_cast<sint>(current_viewport_clip.y),
        current_viewport_clip.z, current_viewport_clip.w);
}

void gearoenix::gl::submission::Manager::set_framebuffer(const uint framebuffer_object)
{
    if (current_bound_framebuffer == framebuffer_object) {
        return;
    }
    current_bound_framebuffer = framebuffer_object;
    if (0 == current_bound_framebuffer) {
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, current_bound_framebuffer);
}

gearoenix::gl::submission::Manager::Manager(Engine& e)
    : e(e)
    , final_shader(new shader::Final())
    , deferred_pbr_shader(e.get_specification().is_deferred_supported ? new shader::DeferredPbr() : nullptr)
    , deferred_pbr_transparent_shader(e.get_specification().is_deferred_supported ? new shader::DeferredPbrTransparent() : nullptr)
    , irradiance_shader(new shader::Irradiance())
    , radiance_shader(new shader::Radiance())
    , skybox_cube_shader(new shader::SkyboxCube())
    , skybox_equirectangular_shader(new shader::SkyboxEquirectangular())
    , ssao_resolve_shader(e.get_specification().is_deferred_supported ? new shader::SsaoResolve() : nullptr)
    , unlit_shader_combination(shader::Manager::get().get<shader::UnlitCombination>())
    , unlit_coloured_shader(unlit_shader_combination->get(false, false, true, false))
    , multiply_shader(new shader::Multiply())
    , bloom_prefilter_shader(new shader::BloomPrefilter())
    , bloom_horizontal_shader(new shader::BloomHorizontal())
    , bloom_vertical_shader(new shader::BloomVertical())
    , bloom_upsampler_shader(new shader::BloomUpsampler())
    , colour_tuning_anti_aliasing_shader_combination(shader::Manager::get().get<shader::ColourTuningAntiAliasingCombination>())
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
    camera_pool.clear();
    scene_pool.clear();
    scenes.clear();

    fill_scenes();
    update_scenes();

    render_shadows();
    render_reflection_probes();

    if (e.get_specification().is_deferred_supported) {
        render_with_deferred();
    } else {
        render_with_forward();
    }
    GX_GL_CHECK_D;

    render_imgui();

#ifdef GX_PLATFORM_INTERFACE_SDL2
    SDL_GL_SwapWindow(e.get_platform_application().get_window());
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
