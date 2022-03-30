#include "gx-gl-submission.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/ecs/gx-cr-ecs-world.hpp"
#include "../core/macro/gx-cr-mcr-profiler.hpp"
#include "../physics/collider/gx-phs-cld-aabb.hpp"
#include "../physics/collider/gx-phs-cld-frustum.hpp"
#include "../physics/gx-phs-transformation.hpp"
#include "../platform/gx-plt-application.hpp"
#include "../platform/macro/gx-plt-mcr-lock.hpp"
#include "../render/camera/gx-rnd-cmr-camera.hpp"
#include "../render/model/gx-rnd-mdl-model.hpp"
#include "../render/scene/gx-rnd-scn-scene.hpp"
#include "gx-gl-camera.hpp"
#include "gx-gl-check.hpp"
#include "gx-gl-constants.hpp"
#include "gx-gl-engine.hpp"
#include "gx-gl-gbuffers-filler-shader.hpp"
#include "gx-gl-loader.hpp"
#include "gx-gl-mesh.hpp"
#include "gx-gl-model.hpp"
#include "gx-gl-texture.hpp"
#include <algorithm>
#include <execution>
#include <thread>

#ifdef GX_PLATFORM_INTERFACE_ANDROID
#include "../platform/android/gx-plt-gl-context.hpp"
#endif

gearoenix::gl::SubmissionManager::CameraData::CameraData() noexcept
    : threads_opaque_models_data(std::thread::hardware_concurrency())
    , threads_tranclucent_models_data(std::thread::hardware_concurrency())
{
}

gearoenix::gl::SubmissionManager::SubmissionManager(Engine& e) noexcept
    : e(e)
    , gbuffers_filler(new GBuffersFillerShader(e))
{
    glClearColor(0.3f, 0.15f, 0.115f, 1.0f);
    // Pipeline settings
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_STENCIL_TEST);
}

gearoenix::gl::SubmissionManager::~SubmissionManager() noexcept = default;

void gearoenix::gl::SubmissionManager::update() noexcept
{
    auto* const world = e.get_world();

    camera_pool.clear();
    scene_pool.clear();
    scenes.clear();

    world->synchronised_system<render::scene::Scene>([&](const core::ecs::Entity::id_t scene_id, render::scene::Scene& scene) {
        if (!scene.enabled)
            return;
        const auto scene_pool_index = scene_pool.emplace([] { return SceneData(); });
        auto& scene_pool_ref = scene_pool[scene_pool_index];
        if (!scenes_bvhs.contains(scene_id))
            scenes_bvhs.emplace(scene_id, physics::accelerator::Bvh<ModelBvhData>());
        scene_pool_ref.cameras.clear();
        world->synchronised_system<render::camera::Camera>([&](const core::ecs::Entity::id_t camera_id, render::camera::Camera& camera) {
            if (!camera.enabled)
                return;
            if (camera.get_scene_id() != scene_id)
                return;
            const auto camera_pool_index = camera_pool.emplace([&] { return CameraData(); });
            scene_pool_ref.cameras.emplace(
                std::make_pair(camera.get_layer(), camera_id),
                camera_pool_index);
        });
        scenes.emplace(std::make_pair(scene.get_layer(), scene_id), scene_pool_index);
    });

    world->parallel_system<render::scene::Scene>([&](const core::ecs::Entity::id_t scene_id, render::scene::Scene& scene, const auto /*kernel_index*/) {
        if (!scene.enabled)
            return;
        auto& scene_data = scene_pool[scenes[std::make_pair(scene.get_layer(), scene_id)]];
        auto& bvh = scenes_bvhs[scene_id];
        if (scene.get_recreate_bvh()) {
            bvh.reset();
            world->synchronised_system<physics::collider::Aabb3, render::model::Model, Model, physics::Transformation>(
                [&](const core::ecs::Entity::id_t, physics::collider::Aabb3& collider, render::model::Model& render_model, Model& model, physics::Transformation& model_transform) {
                    if (!render_model.enabled || render_model.is_transformable || render_model.scene_id != scene_id)
                        return;
                    auto& mesh = *model.bound_mesh;
                    bvh.add({ collider.get_updated_box(),
                        ModelBvhData {
                            .blocked_cameras_flags = render_model.block_cameras_flags,
                            .model = ModelData {
                                .m = math::Mat4x4<float>(model_transform.get_matrix()),
                                .inv_m = math::Mat4x4<float>(model_transform.get_inverted_matrix().transposed()),
                                .albedo_factor = model.material.get_albedo_factor(),
                                .normal_metallic_factor = model.material.get_normal_metallic_factor(),
                                .emission_roughness_factor = model.material.get_emission_roughness_factor(),
                                .vertex_object = mesh.vertex_object,
                                .vertex_buffer = mesh.vertex_buffer,
                                .index_buffer = mesh.index_buffer,
                                .indices_count = mesh.indices_count,
                                .albedo_txt = model.albedo->get_object(),
                                .normal_metallic_txt = model.normal_metallic->get_object(),
                                .emission_roughness_txt = model.emission_roughness->get_object(),
                            } } });
                });
            bvh.create_nodes();
        }
        world->parallel_system<render::camera::Camera, physics::collider::Frustum, physics::Transformation>(
            [&](
                const core::ecs::Entity::id_t camera_id,
                render::camera::Camera& render_camera,
                physics::collider::Frustum& frustum,
                physics::Transformation& transform,
                const auto /*kernel_index*/) noexcept {
                if (!render_camera.get_is_enabled() || scene_id != render_camera.get_scene_id())
                    return;
                const auto camera_location = transform.get_location();
                auto& camera_data = camera_pool[scene_data.cameras[std::make_pair(render_camera.get_layer(), camera_id)]];
                camera_data.vp = render_camera.get_view_projection();
                camera_data.pos = math::Vec3<float>(camera_location);
                camera_data.opaque_models_data.clear();
                camera_data.tranclucent_models_data.clear();
                for (auto& v : camera_data.threads_opaque_models_data)
                    v.clear();
                for (auto& v : camera_data.threads_tranclucent_models_data)
                    v.clear();

                // Recoding static models
                bvh.call_on_intersecting(frustum, [&](const std::remove_reference_t<decltype(bvh)>::Data& bvh_node_data) {
                    if ((bvh_node_data.user_data.blocked_cameras_flags & render_camera.get_flag()) == 0)
                        return;
                    const auto dir = camera_location - bvh_node_data.box.get_center();
                    const auto dis = dir.square_length();
                    camera_data.opaque_models_data.emplace_back(dis, bvh_node_data.user_data.model);
                    // TODO opaque/translucent in ModelBvhData
                });

                // Recording dynamic models
                world->parallel_system<physics::collider::Aabb3, render::model::Model, Model, physics::Transformation>(
                    [&](
                        const core::ecs::Entity::id_t,
                        physics::collider::Aabb3& collider,
                        render::model::Model& render_model,
                        Model& gl_model,
                        physics::Transformation& model_transform,
                        const auto kernel_index) noexcept {
                        if (!render_model.enabled || !render_model.is_transformable || render_model.scene_id != scene_id)
                            return;
                        const auto& box = collider.get_updated_box();
                        if (!frustum.check_intersection(box))
                            return;
                        const auto dir = camera_location - box.get_center();
                        const auto dis = dir.square_length();
                        const auto& mesh = *gl_model.bound_mesh;
                        camera_data.threads_opaque_models_data[kernel_index].emplace_back(
                            dis,
                            ModelData {
                                .m = math::Mat4x4<float>(model_transform.get_matrix()),
                                .inv_m = math::Mat4x4<float>(model_transform.get_inverted_matrix().transposed()),
                                .albedo_factor = gl_model.material.get_albedo_factor(),
                                .normal_metallic_factor = gl_model.material.get_normal_metallic_factor(),
                                .emission_roughness_factor = gl_model.material.get_emission_roughness_factor(),
                                .vertex_object = mesh.vertex_object,
                                .vertex_buffer = mesh.vertex_buffer,
                                .index_buffer = mesh.index_buffer,
                                .indices_count = mesh.indices_count,
                                .albedo_txt = gl_model.albedo->get_object(),
                                .normal_metallic_txt = gl_model.normal_metallic->get_object(),
                                .emission_roughness_txt = gl_model.emission_roughness->get_object(),
                            });
                        // TODO opaque/translucent in ModelBvhData
                    });

                for (auto& v : camera_data.threads_opaque_models_data)
                    std::move(v.begin(), v.end(), std::back_inserter(camera_data.opaque_models_data));

                for (auto& v : camera_data.threads_tranclucent_models_data)
                    std::move(v.begin(), v.end(), std::back_inserter(camera_data.tranclucent_models_data));

                std::sort(
#ifndef GX_PLATFORM_INTERFACE_ANDROID
                    std::execution::par_unseq,
#endif
                    camera_data.opaque_models_data.begin(),
                    camera_data.opaque_models_data.end(),
                    [](const auto& rhs, const auto& lhs) { return rhs.first < lhs.first; });
                std::sort(
#ifndef GX_PLATFORM_INTERFACE_ANDROID
                    std::execution::par_unseq,
#endif
                    camera_data.tranclucent_models_data.begin(),
                    camera_data.tranclucent_models_data.end(),
                    [](const auto& rhs, const auto& lhs) { return rhs.first > lhs.first; });
            });
    });

    auto& os_app = e.get_platform_application();
    const auto& base_os_app = os_app.get_base();

    GX_GL_CHECK_D;
    // Bind target
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(
        static_cast<sizei>(0), static_cast<sizei>(0),
        static_cast<sizei>(base_os_app.get_window_size().x), static_cast<sizei>(base_os_app.get_window_size().y));
    glScissor(
        static_cast<sizei>(0), static_cast<sizei>(0),
        static_cast<sizei>(base_os_app.get_window_size().x), static_cast<sizei>(base_os_app.get_window_size().y));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    gbuffers_filler->bind();
    GX_GL_CHECK_D;
    const auto gbuffers_filler_txt_index_albedo = static_cast<enumerated>(gbuffers_filler->get_albedo_index());
    const auto gbuffers_filler_txt_index_normal_metallic = static_cast<enumerated>(gbuffers_filler->get_normal_metallic_index());
    const auto gbuffers_filler_txt_index_emission_roughness = static_cast<enumerated>(gbuffers_filler->get_emission_roughness_index());

    for (auto& scene_layer_entity_id_pool_index : scenes) {
        auto& scene = scene_pool[scene_layer_entity_id_pool_index.second];
        for (auto& camera_layer_entity_id_pool_index : scene.cameras) {
            GX_GL_CHECK_D;
            auto& camera = camera_pool[camera_layer_entity_id_pool_index.second];
            gbuffers_filler->set_vp_data(reinterpret_cast<const float*>(&camera.vp));
            gbuffers_filler->set_camera_position_data(reinterpret_cast<const float*>(&camera.pos));
            GX_GL_CHECK_D;
            for (auto& distance_model_data : camera.opaque_models_data) {
                GX_GL_CHECK_D;
                auto& model_data = distance_model_data.second;
                gbuffers_filler->set_m_data(reinterpret_cast<const float*>(&model_data.m));
                gbuffers_filler->set_inv_m_data(reinterpret_cast<const float*>(&model_data.inv_m));
                gbuffers_filler->set_albedo_factor_data(reinterpret_cast<const float*>(&model_data.albedo_factor));
                gbuffers_filler->set_normal_metallic_factor_data(reinterpret_cast<const float*>(&model_data.normal_metallic_factor));
                gbuffers_filler->set_emission_roughness_factor_data(reinterpret_cast<const float*>(&model_data.emission_roughness_factor));
                GX_GL_CHECK_D;
                glActiveTexture(GL_TEXTURE0 + gbuffers_filler_txt_index_albedo);
                glBindTexture(GL_TEXTURE_2D, model_data.albedo_txt);
                glActiveTexture(GL_TEXTURE0 + gbuffers_filler_txt_index_normal_metallic);
                glBindTexture(GL_TEXTURE_2D, model_data.normal_metallic_txt);
                glActiveTexture(GL_TEXTURE0 + gbuffers_filler_txt_index_emission_roughness);
                glBindTexture(GL_TEXTURE_2D, model_data.emission_roughness_txt);
                GX_GL_CHECK_D;
                glBindVertexArray(model_data.vertex_object);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model_data.index_buffer);
                glDrawElements(GL_TRIANGLES, model_data.indices_count, GL_UNSIGNED_INT, nullptr);
                GX_GL_CHECK_D;
            }
        }
    }
    GX_GL_CHECK_D;
#ifdef GX_PLATFORM_INTERFACE_SDL2
    SDL_GL_SwapWindow(os_app.get_window());
#elif defined(GX_PLATFORM_INTERFACE_ANDROID)
    os_app.get_gl_context()->swap();
#endif
    GX_GL_CHECK_D;
}

#endif
