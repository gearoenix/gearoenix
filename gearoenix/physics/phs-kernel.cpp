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
    while (alive) {
        signaller->lock();
        const unsigned int threads_count = engine->threads_count;
        std::string s = std::to_string(thread_index) + " of " + std::to_string(threads_count) + "\n";
        std::cout << s;
        const std::vector<std::shared_ptr<render::scene::Scene>>& scenes = engine->render_engine->get_all_scenes();
        for (const std::shared_ptr<render::scene::Scene>& scene : scenes) {
            const std::map<core::Id, std::weak_ptr<render::model::Model>>& models = scene->get_all_models();
            unsigned int model_index = 0;
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
        engine->signaller->release();
    }
    alive = true;
}

gearoenix::physics::Kernel::Kernel(const unsigned int thread_index, Engine* engine)
    : thread_index(thread_index)
    , engine(engine)
{
    signaller = new core::Semaphore();
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
