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
#include <functional>

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

void gearoenix::physics::Engine::update_scenes_kernel(const unsigned int kernel_index) noexcept
{
    GX_START_TASKS
    for (const auto& ls : sorted_scenes) {
        auto * const scene = ls.second;
        if (scene == nullptr || !scene->get_enability())
            continue;
        GX_DO_TASK(scene->update())
        const auto& skies = scene->get_skyboxs();
        for (const auto& sky : skies) {
            if(!sky.second->get_enabled()) continue;
            GX_DO_TASK(sky.second->update())
        }
        const auto& cameras = scene->get_cameras();
        const auto& shadow_cascaders = scene->get_shadow_cascader_lights();
        for (const auto& cam : cameras) {
            auto*const camera = cam.second.get();
            if (camera == nullptr || !camera->get_enabled())
                continue;
            GX_DO_TASK(camera->update())
            if (camera->get_cascaded_shadow_enabled()) {
                for (const auto& id_light : shadow_cascaders) {
                    auto* const light = id_light.second.get();
                    if (!light->get_enabled())
                        continue;
                    GX_DO_TASK(camera->cascade_shadow(light))
                }
            }
        }
    }
}

void gearoenix::physics::Engine::update_visibility_kernel(const unsigned int kernel_index) noexcept
{
    GX_START_TASKS
    for (const auto& layer_scene : sorted_scenes) {
        const auto* const scene = layer_scene.second;
        if (scene == nullptr || !scene->get_enability())
            continue;
        const auto& cameras = scene->get_cameras();
        const auto* const dynamic_accelerator = scene->get_dynamic_accelerator();
        const auto* const static_accelerator = scene->get_static_accelerator();
        for (const auto& id_camera : cameras) {
            auto* const camera = id_camera.second.get();
            if (camera == nullptr || !camera->get_enabled())
                continue;
            if (nullptr != dynamic_accelerator) {
                GX_DO_TASK(camera->check_dynamic_models(dynamic_accelerator))
            }
            if (nullptr != static_accelerator) {
                GX_DO_TASK(camera->check_static_models(static_accelerator))
            }
            auto& cascade_infos = camera->get_cascades();
            for(auto& cascade_info: cascade_infos) {
                if (nullptr != dynamic_accelerator) GX_DO_TASK(cascade_info.shadow(dynamic_accelerator, kernel_index))
                if (nullptr != static_accelerator) GX_DO_TASK(cascade_info.shadow(static_accelerator, kernel_index))
            }
        }
    }
}

void gearoenix::physics::Engine::update_visibility_receiver() noexcept
{
    for (const auto& id_scene : sorted_scenes) {
        const auto*const scene = id_scene.second;
        if (scene == nullptr || !scene->get_enability())
            continue;
        const auto& cameras = scene->get_cameras();
        for (const auto& id_camera : cameras) {
            auto* const camera = id_camera.second.get();
            if (camera == nullptr || !camera->get_enabled())
                continue;
            camera->merge_seen_meshes();
            auto& cascade_infos = camera->get_cascades();
            for(auto& cascade_info: cascade_infos) {
                cascade_info.shrink();
            }
        }
    }
}

gearoenix::physics::Engine::Engine(system::Application* const sys_app, core::sync::KernelWorkers* const workers) noexcept
    : animation_manager(new animation::Manager(workers))
    , sys_app(sys_app)
    , workers(workers)
{
    workers->add_step(std::bind(&Engine::update_scenes_kernel, this, std::placeholders::_1));
    workers->add_step(
        std::bind(&Engine::update_visibility_kernel, this, std::placeholders::_1),
        std::bind(&Engine::update_visibility_receiver, this));
}

void gearoenix::physics::Engine::update() noexcept {
    sorted_scenes.clear();
    const auto& scenes = sys_app->get_asset_manager()->get_scene_manager()->get_scenes();
    for (const auto& id_scene : scenes) {
        const auto scene = id_scene.second.lock();
        if (scene == nullptr || !scene->get_enability())
            continue;
        sorted_scenes.insert({scene->get_layer(), scene.get()});
    }
}

gearoenix::physics::Engine::~Engine() noexcept = default;
