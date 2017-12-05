#include "phs-kernel.hpp"
#include "../core/cr-semaphore.hpp"
#include "../render/camera/rnd-cmr-camera.hpp"
#include "../render/camera/rnd-cmr-orthographic.hpp"
#include "../render/light/rnd-lt-sun.hpp"
#include "../render/model/rnd-mdl-model.hpp"
#include "../render/rnd-engine.hpp"
#include "../render/scene/rnd-scn-scene.hpp"
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
        std::string s = "\n" + std::to_string(thread_index) + " of " + std::to_string(threads_count) + "\n";
        std::cout << s;
        unsigned int model_index = 0;
        const std::vector<std::shared_ptr<render::scene::Scene>>& scenes = engine->render_engine->get_all_scenes();
        for (const std::shared_ptr<render::scene::Scene>& scene : scenes) {
            const std::map<core::Id, std::weak_ptr<render::model::Model>>& models = scene->get_all_models();
            for (const std::pair<core::Id, std::weak_ptr<render::model::Model>>& id_model : models) {
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
#ifdef THREAD_SUPPORTED
        engine->signaller->release();
    }
    alive = true;
#endif
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
    signaller = new core::Semaphore();
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
