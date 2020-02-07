#include "phs-engine.hpp"
#include "../core/asset/cr-asset-manager.hpp"
#include "../core/sync/cr-sync-kernel-workers.hpp"
#include "../math/math-sphere.hpp"
#include "../render/camera/rnd-cmr-camera.hpp"
#include "../render/engine/rnd-eng-engine.hpp"
#include "../render/light/rnd-lt-cascade-info.hpp"
#include "../render/light/rnd-lt-directional.hpp"
#include "../render/material/rnd-mat-material.hpp"
#include "../render/model/rnd-mdl-model.hpp"
#include "../render/scene/rnd-scn-manager.hpp"
#include "../render/scene/rnd-scn-scene.hpp"
#include "../render/skybox/rnd-sky-skybox.hpp"
#include "../system/sys-app.hpp"
#include "accelerator/phs-acc-bvh.hpp"
#include "animation/phs-anm-manager.hpp"
#include "collider/phs-cld-frustum.hpp"
#include <cmath>
#include <functional>
#include <utility>

#define GX_START_TASKS            \
    unsigned int task_number = 0; \
    const unsigned int kernels_count = workers->get_threads_count();

#define GX_DO_TASK(expr)                                 \
    {                                                    \
        if (task_number == kernel_index) {               \
            expr;                                        \
        }                                                \
        task_number = (task_number + 1) % kernels_count; \
    }

gearoenix::physics::Engine::PooledShadowCasterDirectionalLights::PooledShadowCasterDirectionalLights(render::engine::Engine* const e) noexcept
    : cascades_info(new render::light::CascadeInfo(e))
{
}

gearoenix::physics::Engine::PooledShadowCasterDirectionalLights::~PooledShadowCasterDirectionalLights() noexcept = default;

void gearoenix::physics::Engine::update_scenes_kernel(const unsigned int kernel_index) noexcept
{
    GX_START_TASKS
    core::OneLoopPool<PooledSceneData>& kd = kernels_scene_camera_data[kernel_index];
    kd.refresh();
    const std::map<core::Id, std::weak_ptr<render::scene::Scene>>& scenes = sys_app->get_asset_manager()->get_scene_manager()->get_scenes();
    for (const std::pair<const core::Id, std::weak_ptr<render::scene::Scene>>& is : scenes) {
        const std::shared_ptr<render::scene::Scene> scene = is.second.lock();
        if (scene == nullptr || !scene->get_enability())
            continue;
        GX_DO_TASK(scene->update())
        const auto& skies = scene->get_skyboxs();
        for (const auto& sky : skies)
            GX_DO_TASK(sky.second->update())
        auto* const sd = kd.get_next([] { return new PooledSceneData(); });
        sd->scene = scene.get();
        sd->cameras.refresh();
    }
}

void gearoenix::physics::Engine::update_scenes_receiver() noexcept
{
}

void gearoenix::physics::Engine::update_visibility_kernel(const unsigned int kernel_index) noexcept
{
    GX_START_TASKS
    core::OneLoopPool<PooledSceneData>& kd = kernels_scene_camera_data[kernel_index];
    for (PooledSceneData& sd : kd) {
        render::scene::Scene* const scene = sd.scene;
        core::OneLoopPool<PooledCameraData>& cd = sd.cameras;
        const accelerator::Bvh* const dynamic_accelerator = scene->get_dynamic_accelerator();
        const accelerator::Bvh* const static_accelerator = scene->get_static_accelerator();
        const std::map<core::Id, std::shared_ptr<render::camera::Camera>>& cameras = scene->get_cameras();
        const std::map<core::Id, std::shared_ptr<render::light::Light>>& lights = scene->get_lights();
        for (const std::pair<const core::Id, std::shared_ptr<render::camera::Camera>>& id_camera : cameras) {
            render::camera::Camera* const camera = id_camera.second.get();
            if (!camera->get_enabled())
                continue;
            PooledCameraData* const pcd = cd.get_next([] { return new PooledCameraData(); });
            pcd->camera = camera;
            auto& opaque_container_models = pcd->opaque_container_models;
            auto& transparent_container_models = pcd->transparent_container_models;
            auto& shadow_caster_directional_lights = pcd->shadow_caster_directional_lights;
            opaque_container_models.clear();
            transparent_container_models.clear();
            shadow_caster_directional_lights.refresh();
            const std::function<void(collider::Collider* const cld)> collided = [&](collider::Collider* const cld) noexcept {
                auto* const m = cld->get_parent();
                m->update();
                const auto& model_meshes = m->get_meshes();
                for (const auto& model_mesh : model_meshes) {
                    auto* const mat = model_mesh.second->get_mat().get();
                    auto* const msh = model_mesh.second.get();
                    if (mat->get_translucency() == render::material::TranslucencyMode::Transparent) {
                        transparent_container_models[-camera->get_distance(cld->get_location())][mat->get_material_type()][m].push_back(msh);
                    } else {
                        opaque_container_models[mat->get_material_type()][m].push_back(msh);
                    }
                }
            };
            GX_DO_TASK(camera->update_uniform())
            GX_DO_TASK(if (nullptr != dynamic_accelerator) dynamic_accelerator->call_on_intersecting(camera->get_frustum_collider(), collided))
            GX_DO_TASK(if (nullptr != static_accelerator) static_accelerator->call_on_intersecting(camera->get_frustum_collider(), collided))
            const auto& cascade_partitions = camera->get_cascaded_shadow_frustum_partitions();
            for (const std::pair<const core::Id, std::shared_ptr<render::light::Light>>& id_light : lights) {
                auto* const light = id_light.second.get();
                if (!light->is_enabled())
                    continue;
                if (!light->is_shadow_caster())
                    continue;
                auto* const dir_light = dynamic_cast<render::light::Directional*>(light);
                if (dir_light == nullptr)
                    continue;
                GX_DO_TASK(
                    auto* const light_data = shadow_caster_directional_lights.get_next([&] {
                        return new PooledShadowCasterDirectionalLights(sys_app->get_render_engine());
                    });
                    light_data->light = dir_light;
                    auto* const cascade_data = light_data->cascades_info.get();
                    const auto& dir = dir_light->get_direction();
                    const auto dot = std::abs(dir.dot(math::Vec3(0.0f, 1.0f, 0.0f))) - 1.0f;
                    const auto& up = GX_IS_ZERO(dot) ? math::Vec3::Z : math::Vec3::Y;
                    const auto view = math::Mat4x4::look_at(math::Vec3(), dir, up);
                    cascade_data->update(view, cascade_partitions))
            }
        }
    }
}

void gearoenix::physics::Engine::update_visibility_receiver() noexcept
{
    scenes_camera_data.priority_ptr_scene.clear();
    for (const auto& k : kernels_scene_camera_data) {
        for (const auto& s : k) {
            auto* const scene = s.scene;
            auto& scene_camera_data = scenes_camera_data.priority_ptr_scene[scene->get_layer()][scene];
            for (const auto& c : s.cameras) {
                auto* const cam = c.camera;
                auto& camera_data = scene_camera_data.priority_ptr_camera[cam->get_layer()][cam];
                auto& opaque_models = camera_data.opaque_container_models;
                auto& transparent_models = camera_data.transparent_container_models;
                auto& cascades = camera_data.shadow_caster_directional_lights;
                const auto& mo = c.opaque_container_models;
                const auto& mt = c.transparent_container_models;
                const auto& ls = c.shadow_caster_directional_lights;
                for (const auto& material_models : mo) {
                    auto& opaque_models_material = opaque_models[material_models.first];
                    for (const auto& models : material_models.second) {
                        auto& opaque_models_model = opaque_models_material[models.first];
                        opaque_models_model.reserve(models.second.size());
                        for (auto* const m : models.second) {
                            opaque_models_model.push_back(m);
                        }
                    }
                }
                for (const auto& distance_materials : mt) {
                    auto& transparent_models_distance = transparent_models[distance_materials.first];
                    for (const auto& material_models : distance_materials.second) {
                        auto& transparent_models_material = transparent_models_distance[material_models.first];
                        for (const auto& models : material_models.second) {
                            auto& transparent_models_model = transparent_models_material[models.first];
                            transparent_models_model.reserve(models.second.size());
                            for (auto* const m : models.second) {
                                transparent_models_model.push_back(m);
                            }
                        }
                    }
                }
                for (const auto& l : ls) {
                    cascades[l.light->get_layer()][l.light] = l.cascades_info.get();
                }
            }
        }
    }
    for (auto& priority_scene_iter : scenes_camera_data.priority_ptr_scene) {
        for (auto& scene_iter : priority_scene_iter.second) {
            for (auto& priority_camera_iter : scene_iter.second.priority_ptr_camera) {
                for (auto& camera_iter : priority_camera_iter.second) {
                    for (auto material_iter = camera_iter.second.opaque_container_models.begin(); material_iter != camera_iter.second.opaque_container_models.end();) {
                        for (auto model_iter = material_iter->second.begin(); model_iter != material_iter->second.end();) {
                            if (model_iter->second.empty()) {
                                model_iter = material_iter->second.erase(model_iter);
                            } else {
                                ++model_iter;
                            }
                        }
                        if (material_iter->second.empty()) {
                            material_iter = camera_iter.second.opaque_container_models.erase(material_iter);
                        } else {
                            ++material_iter;
                        }
                    }
                    for (auto distance_iter = camera_iter.second.transparent_container_models.begin(); distance_iter != camera_iter.second.transparent_container_models.end();) {
                        for (auto material_iter = distance_iter->second.begin(); material_iter != distance_iter->second.end();) {
                            for (auto model_iter = material_iter->second.begin(); model_iter != material_iter->second.end();) {
                                if (model_iter->second.empty()) {
                                    model_iter = material_iter->second.erase(model_iter);
                                } else {
                                    ++model_iter;
                                }
                            }
                            if (material_iter->second.empty()) {
                                material_iter = camera_iter.second.opaque_container_models.erase(material_iter);
                            } else {
                                ++material_iter;
                            }
                        }
                        if (distance_iter->second.empty()) {
                            distance_iter = camera_iter.second.transparent_container_models.erase(distance_iter);
                        } else {
                            ++distance_iter;
                        }
                    }
                }
            }
        }
    }
}

void gearoenix::physics::Engine::update_shadower_kernel(const unsigned int kernel_index) noexcept
{
    // TODO: I'm not happy with this part it can be much much better, but for this milestone it is ok
    GX_START_TASKS
    for (const auto& priority_scenes_data : scenes_camera_data.priority_ptr_scene) {
        const auto& scenes_data = priority_scenes_data.second;
        for (const auto& scene_data : scenes_data) {
            const auto* const scene = scene_data.first;
            const auto* const static_accelerator = scene->get_static_accelerator();
            const auto* const dynamic_accelerator = scene->get_dynamic_accelerator();
            const auto& priorities_cameras_data = scene_data.second.priority_ptr_camera;
            for (const auto& priority_cameras_data : priorities_cameras_data) {
                const auto& cameras_data = priority_cameras_data.second;
                for (const auto& camera : cameras_data) {
                    const auto& priorities_lights_data = camera.second.shadow_caster_directional_lights;
                    for (const auto& priority_lights_data : priorities_lights_data) {
                        const auto& lights_data = priority_lights_data.second;
                        for (const auto& light_data : lights_data) {
                            auto* const cas = light_data.second;
                            GX_DO_TASK(cas->shadow(static_accelerator, kernel_index))
                            GX_DO_TASK(cas->shadow(dynamic_accelerator, kernel_index))
                        }
                    }
                }
            }
        }
    }
#undef GX_DO_TASK
}

void gearoenix::physics::Engine::update_shadower_receiver() noexcept
{
    for (const auto& priority_scenes_data : scenes_camera_data.priority_ptr_scene) {
        const auto& scenes_data = priority_scenes_data.second;
        for (const auto& scene_data : scenes_data) {
            const auto& priorities_cameras_data = scene_data.second.priority_ptr_camera;
            for (const auto& priority_cameras_data : priorities_cameras_data) {
                const auto& cameras_data = priority_cameras_data.second;
                for (const auto& camera : cameras_data) {
                    const auto& priorities_lights_data = camera.second.shadow_caster_directional_lights;
                    for (const auto& priority_lights_data : priorities_lights_data) {
                        const auto& lights_data = priority_lights_data.second;
                        for (const auto& light_data : lights_data) {
                            auto* const cas = light_data.second;
                            cas->shrink();
                        }
                    }
                }
            }
        }
    }
}

gearoenix::physics::Engine::Engine(system::Application* const sys_app, core::sync::KernelWorkers* const workers) noexcept
    : animation_manager(new animation::Manager(workers))
    , sys_app(sys_app)
    , workers(workers)
    , kernels_scene_camera_data(this->workers->get_threads_count())
{
#define GX_SET_KERNEL(x)                                                      \
    workers->add_step(                                                        \
        std::bind(&Engine::update_##x##_kernel, this, std::placeholders::_1), \
        std::bind(&Engine::update_##x##_receiver, this));

    GX_SET_KERNEL(scenes)
    GX_SET_KERNEL(visibility)
    GX_SET_KERNEL(shadower)
}

gearoenix::physics::Engine::~Engine() noexcept
{
    kernels_scene_camera_data.clear();
}

void gearoenix::physics::Engine::update() noexcept
{
}
