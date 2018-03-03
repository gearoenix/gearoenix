#include "rnd-engine.hpp"
#include "../core/asset/cr-asset-manager.hpp"
#include "../core/cr-end-caller.hpp"
#include "../core/cr-semaphore.hpp"
#include "../core/event/cr-ev-event.hpp"
#include "../core/event/cr-ev-sys-system.hpp"
#include "../physics/phs-engine.hpp"
#include "../system/sys-app.hpp"
#include "../system/sys-log.hpp"
#include "pipeline/rnd-pip-manager.hpp"
#include "scene/rnd-scn-scene.hpp"
#include <functional>
#include <thread>

#ifdef THREAD_SUPPORTED
void gearoenix::render::Engine::scene_loader_function()
{
    while (scene_loader_continue) {
        scene_loader_signaler->lock();
        std::vector<std::function<void()>> temp_functions;
        scene_loader_mutex->lock();
        std::move(scene_loader_functions.begin(), scene_loader_functions.end(), std::back_inserter(temp_functions));
        scene_loader_functions.clear();
        scene_loader_mutex->release();
        for (std::function<void()>& f : temp_functions) {
            f();
        }
    }
}
#endif

gearoenix::render::Engine::Engine(system::Application* system_application)
    : sysapp(system_application)
{
#ifdef THREAD_SUPPORTED
    load_functions_mutex = new core::Semaphore(1);
    loaded_scenes_mutex = new core::Semaphore(1);
    scene_loader_mutex = new core::Semaphore(1);
    scene_loader_signaler = new core::Semaphore();
    scene_loader = std::thread(std::bind(&Engine::scene_loader_function, this));
#endif
    physics_engine = new physics::Engine(this);
}

gearoenix::render::Engine::~Engine()
{
    clear();
}

void gearoenix::render::Engine::clear()
{
    if (pipmgr != nullptr) {
        loaded_scenes.clear();
        delete physics_engine;
        physics_engine = nullptr;
#ifdef THREAD_SUPPORTED
        scene_loader_continue = false;
        scene_loader_signaler->release();
        scene_loader.join();
        delete scene_loader_signaler;
        scene_loader_signaler = nullptr;
        delete scene_loader_mutex;
        scene_loader_mutex = nullptr;
        delete loaded_scenes_mutex;
        loaded_scenes_mutex = nullptr;
        delete load_functions_mutex;
        load_functions_mutex = nullptr;
#endif
        delete pipmgr;
        pipmgr = nullptr;
    }
}

void gearoenix::render::Engine::on_event(core::event::Event& e)
{
    switch (e.get_type()) {
    case core::event::Event::From::SYSTEM: {
        const core::event::system::System& se = e.to_system();
        switch (se.get_action()) {
        case core::event::system::System::Action::UNLOAD: {
            delete pipmgr;
            pipmgr = nullptr;
            loaded_scenes.clear();
            TODO; // it can become better, instead of complete deleting unload it.
            delete physics_engine;
            physics_engine = nullptr;
            load_functions.clear();
#ifdef THREAD_SUPPORTED
            scene_loader_functions.clear();
#endif
            break;
        }
        case core::event::system::System::Action::RELOAD: {

#ifdef THREAD_SUPPORTED
            scene_loader_functions.clear();
#endif
            physics_engine = new physics::Engine(this);
            load_functions.clear();
            loaded_scenes.clear();
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
    for (auto iter = loaded_scenes.begin(); iter != loaded_scenes.end(); ++iter) {
        iter->second->on_event(e);
    }
}

const gearoenix::system::Application* gearoenix::render::Engine::get_system_application() const
{
    return sysapp;
}

gearoenix::system::Application* gearoenix::render::Engine::get_system_application()
{
    return sysapp;
}

const gearoenix::render::pipeline::Manager* gearoenix::render::Engine::get_pipeline_manager() const
{
    return pipmgr;
}

gearoenix::render::pipeline::Manager* gearoenix::render::Engine::get_pipeline_manager()
{
    return pipmgr;
}

void gearoenix::render::Engine::do_load_functions()
{
    std::vector<std::function<void()>> temp_functions;
#ifdef THREAD_SUPPORTED
    load_functions_mutex->lock();
#endif
    std::move(load_functions.begin(), load_functions.end(), std::back_inserter(temp_functions));
    load_functions.clear();
#ifdef THREAD_SUPPORTED
    load_functions_mutex->release();
#endif
    for (std::function<void()>& f : temp_functions) {
        f();
    }
}
void gearoenix::render::Engine::add_load_function(std::function<void()> fun)
{
#ifdef THREAD_SUPPORTED
    load_functions_mutex->lock();
#endif
    load_functions.push_back(fun);
#ifdef THREAD_SUPPORTED
    load_functions_mutex->release();
#endif
}

const std::shared_ptr<gearoenix::render::scene::Scene>& gearoenix::render::Engine::get_scene(core::Id scene_id) const
{
#ifdef DEBUG_MODE
    std::map<core::Id, std::shared_ptr<gearoenix::render::scene::Scene>>::const_iterator f = loaded_scenes.find(scene_id);
    if (f == loaded_scenes.end())
        UNEXPECTED;
    return f->second;
#else
    return loaded_scenes[scene_id];
#endif
}

const std::map<gearoenix::core::Id, std::shared_ptr<gearoenix::render::scene::Scene>>& gearoenix::render::Engine::get_all_scenes() const
{
    return loaded_scenes;
}

void gearoenix::render::Engine::load_scene(core::Id scene_id, std::function<void()> on_load)
{
#ifdef THREAD_SUPPORTED
    scene_loader_mutex->lock();
    scene_loader_functions.push_back([this, scene_id, on_load] {
        loaded_scenes_mutex->lock();
#endif
#ifdef DEBUG_MODE
        if (loaded_scenes.find(scene_id) != loaded_scenes.end())
            UNEXPECTED;
#endif
        sysapp->get_asset_manager()->get_scene(
            scene_id, core::EndCaller<scene::Scene>([this, on_load, scene_id](std::shared_ptr<scene::Scene> asset) -> void {
                loaded_scenes[scene_id] = asset;
                asset->set_renderable(true);
                on_load();
            }));
#ifdef THREAD_SUPPORTED
        loaded_scenes_mutex->release();
    });
    scene_loader_mutex->release();
    scene_loader_signaler->release();
#endif
}

gearoenix::physics::Engine* gearoenix::render::Engine::get_physics_engine()
{
    return physics_engine;
}
