#include "phs-engine.hpp"
#include "../core/asset/cr-asset-manager.hpp"
#include "../core/sync/cr-sync-kernel-workers.hpp"
#include "../core/sync/cr-sync-semaphore.hpp"
#include "../core/sync/cr-sync-stop-point.hpp"
#include "../render/camera/rnd-cmr-camera.hpp"
#include "../render/engine/rnd-eng-engine.hpp"
#include "../render/model/rnd-mdl-model.hpp"
#include "../render/scene/rnd-scn-manager.hpp"
#include "../render/scene/rnd-scn-scene.hpp"
#include "../system/sys-app.hpp"
#include "../system/sys-log.hpp"
#include "animation/phs-anm-animation.hpp"
#include <functional>

void gearoenix::physics::Engine::update_uniform_buffers_kernel(const unsigned int kernel_index)
{
    unsigned int task_number = 0;
    const unsigned int kernels_count = kernels->get_threads_count();
#define GXDOTASK(expr)                 \
    if (task_number == kernel_index) { \
        expr;                          \
    }                                  \
    task_number = (task_number + 1) % kernels_count
    const std::map<core::Id, std::weak_ptr<render::scene::Scene>>& scenes = sysapp->get_asset_manager()->get_scene_manager()->get_scenes();
    for (const std::pair<core::Id, std::weak_ptr<render::scene::Scene>>& is : scenes) {
        if (const std::shared_ptr<render::scene::Scene> scene = is.second.lock()) {
            GXDOTASK(scene->update_uniform());
            const std::map<core::Id, std::shared_ptr<render::camera::Camera>>& cameras = scene->get_cameras();
            for (const std::pair<core::Id, std::shared_ptr<render::camera::Camera>>& id_camera : cameras) {
                GXDOTASK(id_camera.second->update_uniform());
            }
            const std::map<core::Id, std::shared_ptr<render::model::Model>>& models = scene->get_models();
            for (const std::pair<core::Id, std::shared_ptr<render::model::Model>>& id_model : models) {
                GXDOTASK(id_model.second->update_uniform());
            }
        }
    }
}

void gearoenix::physics::Engine::update_uniform_buffers()
{
}

gearoenix::physics::Engine::Engine(const std::shared_ptr<system::Application>& sysapp, const std::shared_ptr<core::sync::KernelWorker> &kernels)
    : sysapp(sysapp)
    , kernels(kernels)
{
    kernels->add_step(std::bind(&Engine::update_uniform_buffers_kernel, this, std::placeholders::_1), std::bind(&Engine::update_uniform_buffers, this));
}

gearoenix::physics::Engine::~Engine()
{
}

void gearoenix::physics::Engine::add_animation(const std::shared_ptr<animation::Animation>& a)
{
    std::lock_guard<std::mutex> _lg(added_animations_locker);
#ifdef GX_DEBUG_MODE
    if (animations.find(a->get_id()) != animations.end())
        GXUNEXPECTED;
#endif
    added_animations.push_back(a);
}

void gearoenix::physics::Engine::remove_animation(const std::shared_ptr<animation::Animation>& a)
{
    remove_animation(a->get_id());
}

void gearoenix::physics::Engine::remove_animation(core::Id a)
{
    std::lock_guard<std::mutex> _lg(removed_animations_locker);
#ifdef GX_DEBUG_MODE
    if (animations.find(a) == animations.end())
        GXUNEXPECTED;
#endif
    removed_animations.push_back(a);
}

void gearoenix::physics::Engine::update()
{
    kernels->do_steps();
}