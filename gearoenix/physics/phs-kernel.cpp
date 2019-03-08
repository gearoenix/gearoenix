#include "phs-kernel.hpp"
#include "../core/sync/cr-sync-semaphore.hpp"
#include "../core/sync/cr-sync-stop-point.hpp"
#include "../render/camera/rnd-cmr-camera.hpp"
#include "../render/camera/rnd-cmr-orthographic.hpp"
#include "../render/engine/rnd-eng-engine.hpp"
#include "../render/light/rnd-lt-sun.hpp"
#include "../render/model/rnd-mdl-model.hpp"
#include "../render/scene/rnd-scn-scene.hpp"
#include "animation/phs-anm-animation.hpp"
#include "body/phs-bd-body.hpp"
#include "body/phs-bd-rigid.hpp"
#include "constraint/phs-cns-constraint.hpp"
#include "phs-engine.hpp"
#include <functional>
#include <iostream>

void gearoenix::physics::Kernel::run()
{
    while (alive) {
        signaller->lock();
        if (!alive)
            break;
        update();
        apply_animations();
        engine->kernels_piont_animations->all_reach();
        apply_constraints();
        engine->kernels_piont_constraints->all_reach();
        apply_bodies();
        engine->kernels_piont_bodies->all_reach();
        apply_models();
        engine->signaller->release();
    }
    alive = true;
}

void gearoenix::physics::Kernel::update()
{
    now = std::chrono::system_clock::now();
    delta_time = std::chrono::duration_cast<std::chrono::duration<core::Real>>(now - last_update).count();
    last_update = now;
}

void gearoenix::physics::Kernel::apply_animations()
{
    const unsigned int threads_count = engine->threads_count;
    unsigned int item_index = 0;
    for (const std::pair<core::Id, std::shared_ptr<animation::Animation>> id_animation : engine->animations) {
        if (((item_index++) % threads_count) != thread_index)
            continue;
        if (id_animation.second->apply(now, delta_time))
            engine->animations_need_cleaning = true;
    }
}

void gearoenix::physics::Kernel::apply_constraints()
{
    const unsigned int threads_count = engine->threads_count;
    unsigned int item_index = 0;
    const std::map<core::Id, std::shared_ptr<render::scene::Scene>>& scenes = engine->render_engine->get_all_scenes();
    for (const std::pair<core::Id, std::shared_ptr<render::scene::Scene>> id_scene : scenes) {
        const std::shared_ptr<render::scene::Scene>& scene = id_scene.second;
        const std::map<core::Id, std::shared_ptr<constraint::Constraint>>& constraints = scene->get_all_root_constraints();
        for (const std::pair<core::Id, std::shared_ptr<constraint::Constraint>> id_constraint : constraints) {
            if (((item_index++) % threads_count) != thread_index)
                continue;
            id_constraint.second->apply(delta_time);
        }
    }
}

void gearoenix::physics::Kernel::apply_bodies()
{
    const unsigned int threads_count = engine->threads_count;
    unsigned int item_index = 0;
    const std::map<core::Id, std::shared_ptr<render::scene::Scene>>& scenes = engine->render_engine->get_all_scenes();
    for (const std::pair<core::Id, std::shared_ptr<render::scene::Scene>> id_scene : scenes) {
        const std::shared_ptr<render::scene::Scene>& scene = id_scene.second;
        const std::map<core::Id, std::weak_ptr<body::Body>>& ids_bodies = scene->get_all_bodies();
        for (const std::pair<core::Id, std::weak_ptr<body::Body>>& id_body : ids_bodies) {
            if (((item_index++) % threads_count) != thread_index)
                continue;
            if (std::shared_ptr<body::Body> b = id_body.second.lock()) {
                if (body::Body::Behaviour::RIGID == b->get_body_type()) {
                    std::shared_ptr<body::Rigid> rigid = std::static_pointer_cast<body::Rigid>(b);
                    rigid->apply_time(delta_time);
                }
            }
        }
    }
}

void gearoenix::physics::Kernel::apply_models()
{
    const unsigned int threads_count = engine->threads_count;
    unsigned int model_index = 0;
    const std::map<core::Id, std::shared_ptr<render::scene::Scene>>& scenes = engine->render_engine->get_all_scenes();
    for (const std::pair<core::Id, std::shared_ptr<render::scene::Scene>> id_scene : scenes) {
        const std::shared_ptr<render::scene::Scene>& scene = id_scene.second;
        const std::map<core::Id, std::weak_ptr<render::model::Model>>& models = scene->get_all_models();
        for (const std::pair<core::Id, std::weak_ptr<render::model::Model>> id_model : models) {
            if (((model_index++) % threads_count) != thread_index)
                continue;
            std::shared_ptr<render::model::Model> model;
            if (!(model = std::get<1>(id_model).lock())) {
                scene->all_models_needs_cleaning = true;
                continue;
            }
            model->commit(scene.get());
        }
    }
}

gearoenix::physics::Kernel::Kernel(const unsigned int thread_index, Engine* engine)
    : engine(engine)
    , thread_index(thread_index)
{
    signaller = new core::sync::Semaphore(1);
    thread = std::thread(std::bind(&Kernel::run, this));
}

gearoenix::physics::Kernel::~Kernel()
{
    alive = false;
    do {
        signaller->release();
        std::this_thread::yield();
    } while (!alive);
    thread.join();
    delete signaller;
}

void gearoenix::physics::Kernel::signal()
{
    signaller->release();
}
