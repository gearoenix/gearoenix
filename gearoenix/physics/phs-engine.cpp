#include "phs-engine.hpp"
#include "../core/asset/cr-asset-manager.hpp"
#include "../core/cr-static.hpp"
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
#include "animation/phs-anm-manager.hpp"
#include <cmath>
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
    auto& kernel_scene_camera_data = kernels_scene_camera_data[kernel_index];
    kernel_scene_camera_data.refresh();
    for (const std::pair<const core::Id, std::weak_ptr<render::scene::Scene>>& is : scenes) {
        const std::shared_ptr<render::scene::Scene> scene = is.second.lock();
        if (scene == nullptr)
            continue;
        if (!scene->get_enability())
            continue;
        auto scene_camera_data = kernel_scene_camera_data.get_next([] { return new SceneCameraData::iterator::value_type; });
        scene_camera_data->first = scene.get();
        auto& cameras_data = scene_camera_data->second;
        cameras_data.refresh();
        GX_DO_TASK(scene->update());
        const std::map<core::Id, std::shared_ptr<render::camera::Camera>>& cameras = scene->get_cameras();
        const std::map<core::Id, std::shared_ptr<render::model::Model>>& models = scene->get_models();
        const std::map<core::Id, std::shared_ptr<render::light::Light>>& lights = scene->get_lights();
        for (const auto& id_model : models) {
            const auto& model = id_model.second;
            if (model->get_enability() == core::State::Set)
                GX_DO_TASK(model->update());
        }
        for (const std::pair<const core::Id, std::shared_ptr<render::camera::Camera>>& id_camera : cameras) {
            const std::shared_ptr<render::camera::Camera>& camera = id_camera.second;
            if (!camera->is_enabled())
                continue;
            auto camera_visible_models = cameras_data.get_next(
                [] { return new decltype(scene_camera_data->second)::iterator::value_type; });
            std::get<0>(*camera_visible_models) = camera.get();
            auto& current_visible_models = std::get<1>(*camera_visible_models);
            current_visible_models.clear();
            auto& lights_cascade_info = std::get<2>(*camera_visible_models);
            lights_cascade_info.refresh();
            GX_DO_TASK(camera->update_uniform());
            const auto& cascade_partitions = camera->get_cascaded_shadow_frustum_partitions();
            for (const std::pair<const core::Id, std::shared_ptr<render::light::Light>>& light : lights) {
                if (!light.second->is_enabled())
                    continue;
                if (!light.second->is_shadow_caster())
                    continue;
                auto dir_light = dynamic_cast<DirLightPtr>(light.second.get());
                if (dir_light == nullptr)
                    continue;
                GX_DO_TASK(
                    auto light_cascaded_shadow_caster_data = lights_cascade_info.get_next([this] {
                        auto p = new DirLightPairedPool<CascadeInfoUPtr>::iterator::value_type;
                        p->second = std::make_unique<render::light::CascadeInfo>(sys_app->get_render_engine());
                        return p;
                    });
                    light_cascaded_shadow_caster_data->first = dir_light;
                    auto* cascade_data = light_cascaded_shadow_caster_data->second.get();
                    const auto& dir = dir_light->get_direction();
                    const auto dot = std::abs(dir.dot(math::Vec3(0.0f, 1.0f, 0.0f))) - 1.0f;
                    const math::Vec3 up = GX_IS_ZERO(dot) ? math::Vec3::Z : math::Vec3::Y;
                    const auto view = math::Mat4x4::look_at(math::Vec3(), dir, up);
                    cascade_data->update(view, cascade_partitions));
            }
            for (const std::pair<const core::Id, std::shared_ptr<render::model::Model>>& id_model : models) {
                const std::shared_ptr<render::model::Model>& model = id_model.second;
                if (model->get_enability() != core::State::Set)
                    continue;
                GXTODO; // object visibility check
            }
        }
    }
}

void gearoenix::physics::Engine::update_001_receiver() noexcept
{
    scenes_camera_data.clear();
    for (auto& k : kernels_scene_camera_data) {
        for (auto& s : k) {
            auto& scene_camera_data = scenes_camera_data[s.first];
            for (auto& c : s.second) {
                auto cam = std::get<0>(c);
                auto& camera_data = scene_camera_data[cam];
                auto& models = camera_data.first;
                auto& cascades = camera_data.second;
                auto& ms = std::get<1>(c);
                auto& ls = std::get<2>(c);
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

void gearoenix::physics::Engine::update_002_kernel(const unsigned int kernel_index) noexcept
{
    unsigned int task_number = 0;
    const unsigned int kernels_count = kernels->get_threads_count();
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

void gearoenix::physics::Engine::update_002_receiver() noexcept
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

gearoenix::physics::Engine::Engine(system::Application* const sysapp, core::sync::KernelWorker* const kernels) noexcept
    : animation_manager(new animation::Manager(kernels))
    , sys_app(sysapp)
    , kernels(kernels)
    , kernels_scene_camera_data(this->kernels->get_threads_count())
{
    this->kernels->add_step(std::bind(&Engine::update_001_kernel, this, std::placeholders::_1), std::bind(&Engine::update_001_receiver, this));
    this->kernels->add_step(std::bind(&Engine::update_002_kernel, this, std::placeholders::_1), std::bind(&Engine::update_002_receiver, this));
}

gearoenix::physics::Engine::~Engine() noexcept
{
    scenes_camera_data.clear();
    kernels_scene_camera_data.clear();
}

void gearoenix::physics::Engine::update() noexcept
{
}

const gearoenix::physics::Engine::GatheredSceneCameraData& gearoenix::physics::Engine::get_visible_models() const noexcept
{
    return scenes_camera_data;
}

gearoenix::physics::Engine::GatheredSceneCameraData& gearoenix::physics::Engine::get_visible_models() noexcept
{
    return scenes_camera_data;
}
