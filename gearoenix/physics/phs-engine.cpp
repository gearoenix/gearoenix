#include "phs-engine.hpp"
#include "../core/asset/cr-asset-manager.hpp"
#include "../core/sync/cr-sync-kernel-workers.hpp"
#include "../core/sync/cr-sync-semaphore.hpp"
#include "../core/sync/cr-sync-stop-point.hpp"
#include "../math/math-sphere.hpp"
#include "../render/camera/rnd-cmr-camera.hpp"
#include "../render/engine/rnd-eng-engine.hpp"
#include "../render/light/rnd-lt-cascade-info.hpp"
#include "../render/light/rnd-lt-directional.hpp"
#include "../render/model/rnd-mdl-model.hpp"
#include "../render/scene/rnd-scn-manager.hpp"
#include "../render/scene/rnd-scn-scene.hpp"
#include "../system/sys-app.hpp"
#include "../system/sys-log.hpp"
#include "animation/phs-anm-animation.hpp"
#include <cstring>
#include <functional>
#include <utility>

void gearoenix::physics::Engine::update_001_kernel(const unsigned int kernel_index) noexcept
{
    unsigned int task_number = 0;
    const unsigned int kernels_count = kernels->get_threads_count();
#define GX_DO_TASK(expr)               \
    if (task_number == kernel_index) { \
        expr;                          \
    }                                  \
    task_number = (task_number + 1) % kernels_count
    const std::map<core::Id, std::weak_ptr<render::scene::Scene>>& scenes = sys_app->get_asset_manager()->get_scene_manager()->get_scenes();
    auto& kernel_visible_models = kernels_visible_models[kernel_index];
    kernel_visible_models.refresh();
    auto& kernel_cascaded_shadow_caster_data = kernels_cascaded_shadow_caster_data[kernel_index];
    kernel_cascaded_shadow_caster_data.refresh();
    for (const std::pair<const core::Id, std::weak_ptr<render::scene::Scene>>& is : scenes) {
        if (const std::shared_ptr<render::scene::Scene> scene = is.second.lock()) {
            if (!scene->is_enabled())
                continue;
            auto scene_visible_models = kernel_visible_models.get_next([] { return new VisibileModels::iterator::value_type; });
            scene_visible_models->first = scene.get();
            auto& cameras_visible_models = scene_visible_models->second;
            cameras_visible_models.refresh();
            auto scene_cascaded_shadow_caster_data = kernel_cascaded_shadow_caster_data.get_next([] { return new SceneCameraDirLightCascadeInfo::iterator::value_type; });
            scene_cascaded_shadow_caster_data->first = scene.get();
            auto& cameras_cascaded_shadow_caster_data = scene_cascaded_shadow_caster_data->second;
            cameras_cascaded_shadow_caster_data.refresh();
            GX_DO_TASK(scene->update_uniform());
            const std::map<core::Id, std::shared_ptr<render::camera::Camera>>& cameras = scene->get_cameras();
            const std::map<core::Id, std::shared_ptr<render::model::Model>>& models = scene->get_models();
            const std::map<core::Id, std::shared_ptr<render::light::Light>>& lights = scene->get_lights();
            for (const std::pair<const core::Id, std::shared_ptr<render::camera::Camera>>& id_camera : cameras) {
                const std::shared_ptr<render::camera::Camera>& camera = id_camera.second;
                if (!camera->is_enabled())
                    continue;
                auto camera_visible_models = cameras_visible_models.get_next([] { return new decltype(scene_visible_models->second)::iterator::value_type; });
                camera_visible_models->first = camera.get();
                auto& current_visible_models = camera_visible_models->second;
                current_visible_models.clear();
                auto camera_cascaded_shadow_caster_data = cameras_cascaded_shadow_caster_data.get_next([] { return new decltype(scene_cascaded_shadow_caster_data->second)::iterator::value_type; });
                camera_cascaded_shadow_caster_data->first = camera.get();
                auto& lights_cascaded_shadow_caster_data = camera_cascaded_shadow_caster_data->second;
                lights_cascaded_shadow_caster_data.refresh();
                GX_DO_TASK(camera->update_uniform());
                const auto& cascade_partitions = camera->get_cascaded_shadow_frustum_partitions();
                for (const std::pair<const core::Id, std::shared_ptr<render::light::Light>>& light : lights) {
                    if (!light.second->is_enabled())
                        continue;
                    if (!light.second->is_shadower())
                        continue;
                    const auto dir_light = std::dynamic_pointer_cast<render::light::Directional>(light.second);
                    if (dir_light == nullptr)
                        continue;
                    GX_DO_TASK(
                        auto light_cascaded_shadow_caster_data = lights_cascaded_shadow_caster_data.get_next([this] {
                            auto p = new decltype(camera_cascaded_shadow_caster_data->second)::iterator::value_type;
                            p->second = new render::light::CascadeInfo(sys_app->get_render_engine().get());
                            return p;
                        });
                        light_cascaded_shadow_caster_data->first = dir_light.get();
                        auto& cascade_data = light_cascaded_shadow_caster_data->second;
                        //camera_light_info[dir_light.get()] = dir_light->create_cascades_info(camera.second)
                    );
                }
                for (const std::pair<const core::Id, std::shared_ptr<render::model::Model>>& id_model : models) {
                    const std::shared_ptr<render::model::Model>& model = id_model.second;
                    if (!model->is_enabled())
                        continue;
                    GX_DO_TASK(
                        const math::Sphere& sphere = model->get_occlusion_sphere();
                        if (camera->in_sight(sphere.position, sphere.radius)) {
                            current_visible_models.push_back(model.get());
                        });
                }
            }
            for (const std::pair<const core::Id, std::shared_ptr<render::model::Model>>& id_model : models) {
                const std::shared_ptr<render::model::Model>& model = id_model.second;
                if (!model->is_enabled())
                    continue;
                GX_DO_TASK(model->update_uniform());
            }
        }
    }
}

void gearoenix::physics::Engine::update_001_receiver() noexcept
{
    visible_models.clear();
    for (auto& k : kernels_visible_models) {
        for (auto& s : k) {
            auto& scene_visible_models = visible_models[s.first];
            for (auto& c : s.second) {
                auto& models = scene_visible_models[c.first];
                for (auto m : c.second)
                    models.push_back(m);
            }
        }
    }
    //    cascaded_shadows_partitions.clear();
    //    for (auto& k : kernels_cascaded_shadows_partitions) {
    //        for (auto& s : k) {
    //            cascaded_shadows_partitions[s.first].merge(s.second);
    //        }
    //        k.clear();
    //    }
}

void gearoenix::physics::Engine::update_002_kernel(const unsigned int kernel_index) noexcept
{
    //    unsigned int task_number = 0;
    //    const unsigned int kernels_count = kernels->get_threads_count();
    //    SceneCameraLightCascadeInfo& kernel_camera_light_info = kernels_cascaded_shadow_caster_data[kernel_index];
    //    for (const std::pair<const std::shared_ptr<render::scene::Scene>, CameraPartitions>& scene_partitions : cascaded_shadows_partitions) {
    //        const std::shared_ptr<render::scene::Scene>& scene = scene_partitions.first;
    //        if (!scene->is_enabled())
    //            continue;
    //        const CameraPartitions& cameras = scene_partitions.second;
    //        CameraLightCascadeInfo& scene_camera_light_info = kernel_camera_light_info[scene];
    //        for (const std::pair<render::camera::Camera* const, Partitions>& camera : cameras) {
    //            const std::map<core::Id, std::shared_ptr<render::light::Light>>& lights = scene->get_lights();
    //            LightCascadeInfo& camera_light_info = scene_camera_light_info[camera.first];
    //        }
    //    }
    //#undef GX_DO_TASK
}

void gearoenix::physics::Engine::update_002_receiver() noexcept
{
    //    cascaded_shadow_caster_data.clear();
    //    for (SceneCameraLightCascadeInfo& k : kernels_cascaded_shadow_caster_data) {
    //        for (std::pair<const std::shared_ptr<render::scene::Scene>, CameraLightCascadeInfo>& s : k) {
    //            CameraLightCascadeInfo& scene_info = cascaded_shadow_caster_data[s.first];
    //            for (std::pair<render::camera::Camera* const, LightCascadeInfo>& c : s.second) {
    //                scene_info[c.first].merge(c.second);
    //            }
    //        }
    //        k.clear();
    //    }
}

gearoenix::physics::Engine::Engine(system::Application* const sysapp, core::sync::KernelWorker* const kernels) noexcept
    : sys_app(sysapp)
    , kernels(kernels)
    , kernels_visible_models(this->kernels->get_threads_count())
    , kernels_cascaded_shadow_caster_data(this->kernels->get_threads_count())
{
    this->kernels->add_step(std::bind(&Engine::update_001_kernel, this, std::placeholders::_1), std::bind(&Engine::update_001_receiver, this));
    this->kernels->add_step(std::bind(&Engine::update_002_kernel, this, std::placeholders::_1), std::bind(&Engine::update_002_receiver, this));
}

void gearoenix::physics::Engine::add_animation(const std::shared_ptr<animation::Animation>& a) noexcept
{
    std::lock_guard<std::mutex> _lg(added_animations_locker);
#ifdef GX_DEBUG_MODE
    if (animations.find(a->get_id()) != animations.end())
        GXUNEXPECTED
#endif
    added_animations.push_back(a);
}

void gearoenix::physics::Engine::remove_animation(const std::shared_ptr<animation::Animation>& a) noexcept
{
    remove_animation(a->get_id());
}

void gearoenix::physics::Engine::remove_animation(core::Id a) noexcept
{
    std::lock_guard<std::mutex> _lg(removed_animations_locker);
#ifdef GX_DEBUG_MODE
    if (animations.find(a) == animations.end())
        GXUNEXPECTED
#endif
    removed_animations.push_back(a);
}

void gearoenix::physics::Engine::update() noexcept
{
}

const gearoenix::physics::Engine::GatheredVisibileModels& gearoenix::physics::Engine::get_visible_models() const noexcept
{
    return visible_models;
}

gearoenix::physics::Engine::GatheredVisibileModels& gearoenix::physics::Engine::get_visible_models() noexcept
{
    return visible_models;
}
