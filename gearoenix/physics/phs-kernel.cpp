#include "phs-kernel.hpp"
#include "../core/sync/cr-sync-semaphore.hpp"
#include "../render/camera/rnd-cmr-camera.hpp"
#include "../render/camera/rnd-cmr-orthographic.hpp"
#include "../render/light/rnd-lt-sun.hpp"
#include "../render/model/rnd-mdl-model.hpp"
#include "../render/rnd-engine.hpp"
#include "../render/scene/rnd-scn-scene.hpp"
#include "animation/phs-anm-animation.hpp"
#include "constraint/phs-cns-constraint.hpp"
#include "phs-engine.hpp"
#include <functional>
#include <iostream>

void gearoenix::physics::Kernel::run()
{
    while (alive) {
        signaller->lock();
        const unsigned int threads_count = engine->threads_count;
        now = std::chrono::system_clock::now();
        delta_time = std::chrono::duration_cast<std::chrono::duration<core::Real>>(now - last_update).count();
        // std::string s = "\n" + std::to_string(thread_index) + " of " + std::to_string(threads_count) + "\n";
        // std::cout << s;
        apply_animations();
        apply_constraints();
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
        last_update = now;
        engine->signaller->release();
    }
    alive = true;
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
