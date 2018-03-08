#include "phs-kernel.hpp"
#include "../core/cr-semaphore.hpp"
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
#ifdef THREAD_SUPPORTED
    while (alive) {
        signaller->lock();
        const unsigned int threads_count = engine->threads_count;
#else
    const unsigned int threads_count = 1;
    const unsigned int thread_index = 0;
#endif
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
#ifdef THREAD_SUPPORTED
        engine->signaller->release();
    }
    alive = true;
#endif
}

void gearoenix::physics::Kernel::apply_animations()
{
#ifdef THREAD_SUPPORTED
    const unsigned int threads_count = engine->threads_count;
    unsigned int item_index = 0;
#endif
    for (const std::pair<core::Id, std::shared_ptr<animation::Animation>> id_animation : engine->animations) {
#ifdef THREAD_SUPPORTED
        if (((item_index++) % threads_count) != thread_index)
            continue;
#endif
        if (id_animation.second->apply(now, delta_time))
            engine->animations_need_cleaning = true;
    }
}

void gearoenix::physics::Kernel::apply_constraints()
{
#ifdef THREAD_SUPPORTED
    const unsigned int threads_count = engine->threads_count;
    unsigned int item_index = 0;
#endif
    const std::map<core::Id, std::shared_ptr<render::scene::Scene>>& scenes = engine->render_engine->get_all_scenes();
    for (const std::pair<core::Id, std::shared_ptr<render::scene::Scene>> id_scene : scenes) {
        const std::shared_ptr<render::scene::Scene>& scene = id_scene.second;
        const std::map<core::Id, std::shared_ptr<constraint::Constraint>>& constraints = scene->get_all_root_constraints();
        for (const std::pair<core::Id, std::shared_ptr<constraint::Constraint>> id_constraint : constraints) {
#ifdef THREAD_SUPPORTED
            if (((item_index++) % threads_count) != thread_index)
                continue;
#endif
            id_constraint.second->apply();
        }
    }
}

gearoenix::physics::Kernel::Kernel(
#ifdef THREAD_SUPPORTED
    const unsigned int thread_index,
#endif
    Engine* engine)
    : engine(engine)
#ifdef THREAD_SUPPORTED
    , thread_index(thread_index)
#endif
{
#ifdef THREAD_SUPPORTED
    signaller = new core::Semaphore(1);
    thread = std::thread(std::bind(&Kernel::run, this));
#endif
}

gearoenix::physics::Kernel::~Kernel()
{
#ifdef THREAD_SUPPORTED
    alive = false;
    do {
        signaller->release();
        std::this_thread::yield();
    } while (!alive);
    thread.join();
    delete signaller;
#endif
}

void gearoenix::physics::Kernel::signal()
{
#ifdef THREAD_SUPPORTED
    signaller->release();
#else
        run();
#endif
}
