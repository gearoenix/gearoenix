#include "phs-engine.hpp"
#include "../core/asset/cr-asset-manager.hpp"
#include "../core/sync/cr-sync-kernel-workers.hpp"
#include "../math/math-sphere.hpp"
#include "../render/camera/rnd-cmr-camera.hpp"
#include "../render/engine/rnd-eng-engine.hpp"
#include "../render/light/rnd-lt-cascade-info.hpp"
#include "../render/light/rnd-lt-directional.hpp"
#include "../render/model/rnd-mdl-model.hpp"
#include "../render/scene/rnd-scn-manager.hpp"
#include "../render/scene/rnd-scn-scene.hpp"
#include "../system/sys-app.hpp"
#include "accelerator/phs-acc-bvh.hpp"
#include "animation/phs-anm-manager.hpp"
#include <cmath>
#include <functional>
#include <utility>

#define GX_START_TASKS            \
    unsigned int task_number = 0; \
    const unsigned int kernels_count = workers->get_threads_count()
#define GX_DO_TASK(expr)               \
    if (task_number == kernel_index) { \
        expr                           \
    }                                  \
    task_number = (task_number + 1) % kernels_count


gearoenix::physics::Engine::PooledShadowCasterDirectionalLights::PooledShadowCasterDirectionalLights(render::engine::Engine * const e) noexcept
    : cascades_info(new render::light::CascadeInfo(e)) {}

gearoenix::physics::Engine::PooledShadowCasterDirectionalLights::~PooledShadowCasterDirectionalLights() noexcept = default;

void gearoenix::physics::Engine::update_scenes_kernel(const unsigned int kernel_index) noexcept
{
    GX_START_TASKS;
    core::OneLoopPool<PooledSceneData>& kd = kernels_scene_camera_data[kernel_index];
    kd.refresh();
    const std::map<core::Id, std::weak_ptr<render::scene::Scene>>& scenes = sys_app->get_asset_manager()->get_scene_manager()->get_scenes();
    for (const std::pair<const core::Id, std::weak_ptr<render::scene::Scene>>& is : scenes) {
        GX_DO_TASK (
            const std::shared_ptr<render::scene::Scene> scene = is.second.lock();
            if (scene != nullptr && scene->get_enability()) {
                scene->update();
            }
            auto* const sd = kd.get_next([] { return new PooledSceneData(); });
            sd->scene = scene.get();
            sd->cameras.refresh();
        );
    }
}

void gearoenix::physics::Engine::update_scenes_receiver() noexcept { }

void gearoenix::physics::Engine::update_visibility_kernel(const unsigned int kernel_index) noexcept
{
    GX_START_TASKS;
    core::OneLoopPool<PooledSceneData>& kd = kernels_scene_camera_data[kernel_index];
    for(PooledSceneData& sd: kd) {
        render::scene::Scene* scene = sd.scene;
        core::OneLoopPool<PooledCameraData>& cd = sd.cameras;
        const accelerator::Bvh * const dynamic_accelerator = scene->get_static_accelerator();
        const accelerator::Bvh * const static_accelerator = scene->get_static_accelerator();
        const std::map<core::Id, std::shared_ptr<render::camera::Camera>>& cameras = scene->get_cameras();
        const std::map<core::Id, std::shared_ptr<render::model::Model>>& models = scene->get_models();
        const std::map<core::Id, std::shared_ptr<render::light::Light>>& lights = scene->get_lights();
        for (const std::pair<const core::Id, std::shared_ptr<render::camera::Camera>>& id_camera : cameras) {
            render::camera::Camera* const camera = id_camera.second.get();
            if (!camera->is_enabled())
                continue;
            PooledCameraData *const pcd = cd.get_next([] { return new PooledCameraData(); });
            pcd->camera = camera;
            auto& opaque_container_models = pcd->opaque_container_models;
            auto& transparent_container_models = pcd->transparent_container_models;
            auto& shadow_caster_directional_lights = pcd->shadow_caster_directional_lights;
            opaque_container_models.clear();
            transparent_container_models.clear();
            shadow_caster_directional_lights.refresh();
#define GX_CAMVIS(x) \
            GX_DO_TASK ( \
                    x##_accelerator->call_on_intersecting( \
                            camera->get_collider(), [&](collider::Collider* const cld) noexcept { \
            } ))
            GX_DO_TASK(camera->update_uniform(););
            GX_CAMVIS(dynamic);
            GX_CAMVIS(static);
            const auto& cascade_partitions = camera->get_cascaded_shadow_frustum_partitions();
            for (const std::pair<const core::Id, std::shared_ptr<render::light::Light>>& id_light : lights) {
                GX_DO_TASK(
                auto * const light = id_light.second.get();
                if (!light->is_enabled())
                    continue;
                if (!light->is_shadow_caster())
                    continue;
                auto* const dir_light = dynamic_cast<render::light::Directional*>(light);
                if (dir_light == nullptr)
                    continue;
                    auto * const light_data = shadow_caster_directional_lights.get_next([&] {
                        return new PooledShadowCasterDirectionalLights(sys_app->get_render_engine());
                    });
                    light_data->light = dir_light;
                    auto* const cascade_data = light_data->cascades_info.get();
                    const auto& dir = dir_light->get_direction();
                    const auto dot = std::abs(dir.dot(math::Vec3(0.0f, 1.0f, 0.0f))) - 1.0f;
                    const math::Vec3 up = GX_IS_ZERO(dot) ? math::Vec3::Z : math::Vec3::Y;
                    const auto view = math::Mat4x4::look_at(math::Vec3(), dir, up);
                    cascade_data->update(view, cascade_partitions);
                );
            }
        }
    }
}

void gearoenix::physics::Engine::update_visibility_receiver() noexcept
{
    scenes_camera_data.priority_ptr_scene.clear();
    for (auto& k : kernels_scene_camera_data) {
        for (auto& s : k) {
            auto * const scene = s.scene;
            auto& scene_camera_data = scenes_camera_data.priority_ptr_scene[scene->get_layer()][scene];
            for (auto& c : s.cameras) {
                auto * const cam = c.camera;
                auto& camera_data = scene_camera_data.priority_ptr_camera[cam->get_layer()][cam];
                auto& opaque_models = camera_data.opaque_container_models;
                auto& transparent_models = camera_data.transparent_container_models;
                auto& cascades = camera_data.shadow_caster_directional_lights;
                auto& mo = c.opaque_container_models;
                auto& mt = c.transparent_container_models;
                auto& ls = c.shadow_caster_directional_lights;
                for (auto& m : ms)
                    models.push_back(m);
                for (auto& l : ls) {
                    l.second->start();
                    cascades.emplace_back(l.first, l.second.get());
                }
            }
        }
    }
}

void gearoenix::physics::Engine::update_shadower_kernel(const unsigned int kernel_index) noexcept
{
    GX_START_TASKS
    for (auto& scene_camera_data : scenes_camera_data) {
        ScenePtr scene = scene_camera_data.first;
        auto& cameras_data = scene_camera_data.second;
        auto& models = scene->get_models();
        for (auto& camera : cameras_data) {
            auto& lights = camera.second.second;
            for (auto& lc : lights) {
                auto* cas = lc.second;
                for (auto& im : models) {
                    auto& m = im.second;
                    if (m->get_enability() != core::State::Set || m->get_shadowing() != core::State::Set)
                        continue;
                    GX_DO_TASK(cas->shadow(m.get(), task_number));
                }
            }
        }
    }
#undef GX_DO_TASK
}

void gearoenix::physics::Engine::update_shadower_receiver() noexcept
{
    for (auto& scene_camera_data : scenes_camera_data) {
        auto& cameras_data = scene_camera_data.second;
        for (auto& camera : cameras_data) {
            auto& lights = camera.second.second;
            for (auto& lc : lights) {
                auto* cas = lc.second;
                cas->shrink();
            }
        }
    }
}

gearoenix::physics::Engine::Engine(system::Application* const sysapp, core::sync::KernelWorkers* const workers) noexcept
    : animation_manager(new animation::Manager(workers))
    , sys_app(sysapp)
    , workers(workers)
    , kernels_scene_camera_data(this->workers->get_threads_count())
{
#define GX_SET_KERNEL(x)                                                      \
    workers->add_step(                                                        \
        std::bind(&Engine::update_##x##_sender, this),                        \
        std::bind(&Engine::update_##x##_kernel, this, std::placeholders::_1), \
        std::bind(&Engine::update_##x##_meanwhile, this),                     \
        std::bind(&Engine::update_##x##_receiver, this));

    GX_SET_KERNEL(scenes)
    GX_SET_KERNEL(visibility)
    GX_SET_KERNEL(shadower)
}

gearoenix::physics::Engine::~Engine() noexcept
{
    scenes_camera_data.clear();
    kernels_scene_camera_data.clear();
}

void gearoenix::physics::Engine::update() noexcept
{
    update_scenes();
}

const gearoenix::physics::Engine::GatheredSceneCameraData& gearoenix::physics::Engine::get_visible_models() const noexcept
{
    return scenes_camera_data;
}

gearoenix::physics::Engine::GatheredSceneCameraData& gearoenix::physics::Engine::get_visible_models() noexcept {
    return scenes_camera_data;
}