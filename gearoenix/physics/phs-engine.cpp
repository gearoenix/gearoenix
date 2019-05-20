#include "phs-engine.hpp"
#include "../core/asset/cr-asset-manager.hpp"
#include "../core/sync/cr-sync-kernel-workers.hpp"
#include "../core/sync/cr-sync-semaphore.hpp"
#include "../core/sync/cr-sync-stop-point.hpp"
#include "../math/math-sphere.hpp"
#include "../render/camera/rnd-cmr-camera.hpp"
#include "../render/engine/rnd-eng-engine.hpp"
#include "../render/model/rnd-mdl-model.hpp"
#include "../render/scene/rnd-scn-manager.hpp"
#include "../render/scene/rnd-scn-scene.hpp"
#include "../render/light/rnd-lt-directional.hpp"
#include "../system/sys-app.hpp"
#include "../system/sys-log.hpp"
#include "animation/phs-anm-animation.hpp"
#include <functional>
#include <utility>

void gearoenix::physics::Engine::update_001_kernel(const unsigned int kernel_index) noexcept
{
    unsigned int task_number = 0;
    const unsigned int kernels_count = kernels->get_threads_count();
#define GXDOTASK(expr)                 \
    if (task_number == kernel_index) { \
        expr;                          \
    }                                  \
    task_number = (task_number + 1) % kernels_count
    const std::map<core::Id, std::weak_ptr<render::scene::Scene>>& scenes = sysapp->get_asset_manager()->get_scene_manager()->get_scenes();
    for (const std::pair<const core::Id, std::weak_ptr<render::scene::Scene>>& is : scenes) {
        if (const std::shared_ptr<render::scene::Scene> scene = is.second.lock()) {
            if(!scene->is_renderable()) continue;
            GXDOTASK(scene->update_uniform());
            const std::map<core::Id, std::shared_ptr<render::camera::Camera>>& cameras = scene->get_cameras();
            const std::map<core::Id, std::shared_ptr<render::model::Model>>& models = scene->get_models();
            for (const std::pair<const core::Id, std::shared_ptr<render::camera::Camera>>& id_camera : cameras) {
                const std::shared_ptr<render::camera::Camera>& camera = id_camera.second;
                // TODO: renderable check for camera
                GXDOTASK(camera->update_uniform());
                GXDOTASK(kernels_cascaded_shadows_partitions[kernel_index][scene][camera] = camera->get_cascaded_shadow_frustum_partitions());
                for (const std::pair<const core::Id, std::shared_ptr<render::model::Model>>& id_model : models) {
                    const std::shared_ptr<render::model::Model>& model = id_model.second;
                    // TODO: renderable check model
                    GXDOTASK(
                        const math::Sphere& sphere = model->get_occlusion_sphere();
                        if (camera->in_sight(sphere.position, sphere.radius)) {
                            kernels_visible_models[kernel_index][scene][camera].insert(model);
                        });
                }
            }
            for (const std::pair<const core::Id, std::shared_ptr<render::model::Model>>& id_model : models) {
                // TODO: renderable check model
                GXDOTASK(id_model.second->update_uniform());
            }
        }
    }
}

void gearoenix::physics::Engine::update_001_receiver()noexcept
{
    visible_models.clear();
    for (auto& k : kernels_visible_models) {
        for (auto& s : k) {
            for (auto& c : s.second) {
                visible_models[s.first][c.first].merge(c.second);
            }
        }
        k.clear();
    }
    cascaded_shadows_partitions.clear();
    for (auto& k : kernels_cascaded_shadows_partitions) {
        for (auto& s : k) {
            cascaded_shadows_partitions[s.first].merge(s.second);
        }
        k.clear();
    }
}

void gearoenix::physics::Engine::update_002_kernel(const unsigned int kernel_index)noexcept
{
    unsigned int task_number = 0;
    const unsigned int kernels_count = kernels->get_threads_count();
    SceneCameraLightCascadeInfo &kernel_camera_light_info = kernels_cascaded_shadow_caster_data[kernel_index];
    for (const std::pair<const std::shared_ptr<render::scene::Scene>, CameraPartitions>& scene_partitions : cascaded_shadows_partitions) {
        const std::shared_ptr<render::scene::Scene> &scene = scene_partitions.first;
        if (!scene->is_renderable()) continue;
        const CameraPartitions &cameras = scene_partitions.second;
        for (const std::pair<const std::shared_ptr<render::camera::Camera>, Partitions> &camera: cameras) {
            const std::map<core::Id, std::shared_ptr<render::light::Light>> &lights = scene->get_lights();
            for (const std::pair<const core::Id, std::shared_ptr<render::light::Light>> &light: lights) {
                if (!light.second->is_enabled()) continue;
                if (!light.second->is_shadower()) continue;
                const auto dir_light = std::dynamic_pointer_cast<render::light::Directional>(light.second);
                if (dir_light == nullptr) continue;
                GXDOTASK(
                        /// Following searches are for using less allocation, and std::shared_ptr constructor
                        // TODO: improve
                        const SceneCameraLightCascadeInfo::iterator &search1 = kernel_camera_light_info.find(scene);
                        if(search1 == kernel_camera_light_info.end()) {
                            // kernel_camera_light_info[scene][camera.first][dir_light] = dir_light->create_cascades_info(camera.second);
                        } else {
                            const CameraLightCascadeInfo::iterator &search2 = search1->second.find(camera.first);
                            if (search2 == search1->second.end()) {
                                // search1->second[camera.first][dir_light] = dir_light->create_cascades_info(camera.second);
                            } else {
                                const LightCascadeInfo::iterator &search3 = search2->second.find(dir_light);
                                if (search3 == search2->second.end()) {
                                    // search2->second[dir_light] = dir_light->create_cascades_info(camera.second);
                                } else {
                                    // dir_light->update_cascades_info(search3->second, camera.second);
                                }
                            }
                        }
                );
            }
        }
    }
#undef GXDOTASK
}

void gearoenix::physics::Engine::update_002_receiver()noexcept
{
    cascaded_shadow_caster_data.clear();
    for (SceneCameraLightCascadeInfo& k : kernels_cascaded_shadow_caster_data) {
        for (std::pair<const std::shared_ptr<render::scene::Scene>, CameraLightCascadeInfo>& s : k) {
            CameraLightCascadeInfo &scene_info = cascaded_shadow_caster_data[s.first];
            for (std::pair<const std::shared_ptr<render::camera::Camera>, LightCascadeInfo>& c : s.second) {
                scene_info[c.first].merge(c.second);
            }
        }
        k.clear();
    }
}

gearoenix::physics::Engine::Engine(std::shared_ptr<system::Application> sysapp, std::shared_ptr<core::sync::KernelWorker> kernels) noexcept
    : sysapp(std::move(sysapp))
    , kernels(std::move(kernels))
        , kernels_visible_models(this->kernels->get_threads_count())
        , kernels_cascaded_shadows_partitions(this->kernels->get_threads_count())
        , kernels_cascaded_shadow_caster_data(this->kernels->get_threads_count())
{
    this->kernels->add_step(std::bind(&Engine::update_001_kernel, this, std::placeholders::_1), std::bind(
            &Engine::update_001_receiver, this));
    this->kernels->add_step(std::bind(&Engine::update_002_kernel, this, std::placeholders::_1), std::bind(
            &Engine::update_002_receiver, this));
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

const gearoenix::physics::Engine::SceneCameraModelSet& gearoenix::physics::Engine::get_visible_models() const noexcept
{
    return visible_models;
}

gearoenix::physics::Engine::SceneCameraModelSet& gearoenix::physics::Engine::get_visible_models() noexcept
{
    return visible_models;
}
