#include "rnd-eng-engine.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/event/cr-ev-event.hpp"
#include "../../core/event/cr-ev-sys-system.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../core/sync/cr-sync-semaphore.hpp"
#include "../../physics/phs-engine.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-log.hpp"
#include "../pipeline/rnd-pip-manager.hpp"
#include "../scene/rnd-scn-manager.hpp"
#include "../scene/rnd-scn-scene.hpp"
#include <functional>
#include <thread>

void gearoenix::render::engine::Engine::scene_loader_function()
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

gearoenix::render::engine::Engine::Engine(const std::shared_ptr<system::Application>& system_application, const Type::Id engine_type_id)
    : sysapp(system_application)
    , engine_type_id(engine_type_id)
{
    load_functions_mutex = new core::sync::Semaphore(1);
    loaded_scenes_mutex = new core::sync::Semaphore(1);
    scene_loader_mutex = new core::sync::Semaphore(1);
    scene_loader_signaler = new core::sync::Semaphore();
    scene_loader = std::thread(std::bind(&Engine::scene_loader_function, this));
    //physics_engine = new physics::Engine(this);
}

gearoenix::render::engine::Engine::~Engine()
{
    clear();
}

void gearoenix::render::engine::Engine::clear()
{
    if (pipeline_manager != nullptr) {
        delete physics_engine;
        physics_engine = nullptr;
        loaded_scenes.clear();
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
        // delete pipmgr;
        //pipmgr = nullptr;
    }
}

//void gearoenix::render::engine::Engine::on_event(core::event::Event& e)
//{
//    switch (e.get_type()) {
//    case core::event::Event::From::SYSTEM: {
//        const core::event::system::System& se = e.to_system();
//        switch (se.get_action()) {
//        case core::event::system::System::Action::UNLOAD: {
//            delete pipmgr;
//            pipmgr = nullptr;
//            loaded_scenes.clear();
//            GXTODO; // it can become better, instead of complete deleting unload it.
//            delete physics_engine;
//            physics_engine = nullptr;
//            load_functions.clear();
//            scene_loader_functions.clear();
//            break;
//        }
//        case core::event::system::System::Action::RELOAD: {
//            scene_loader_functions.clear();
//            physics_engine = new physics::Engine(this);
//            load_functions.clear();
//            loaded_scenes.clear();
//            break;
//        }
//        default:
//            break;
//        }
//        break;
//    }
//    default:
//        break;
//    }
//    for (auto iter = loaded_scenes.begin(); iter != loaded_scenes.end(); ++iter) {
//        iter->second->on_event(e);
//    }
//}

//const command::Manager* gearoenix::render::engine::Engine::get_command_manager() const
//{
//    return command_manager;
//}
//
//command::Manager* gearoenix::render::engine::Engine::get_command_manager()
//{
//    return command_manager;
//}
//
//const gearoenix::system::Application* gearoenix::render::engine::Engine::get_system_application() const
//{
//    return sysapp;
//}
//
//gearoenix::system::Application* gearoenix::render::engine::Engine::get_system_application()
//{
//    return sysapp;
//}
//
//const gearoenix::render::pipeline::Manager* gearoenix::render::engine::Engine::get_pipeline_manager() const
//{
//    return pipmgr;
//}
//
//gearoenix::render::pipeline::Manager* gearoenix::render::engine::Engine::get_pipeline_manager()
//{
//    return pipmgr;
//}

void gearoenix::render::engine::Engine::do_load_functions()
{
    std::vector<std::function<void()>> temp_functions;
    load_functions_mutex->lock();
    std::move(load_functions.begin(), load_functions.end(), std::back_inserter(temp_functions));
    load_functions.clear();
    load_functions_mutex->release();
    for (std::function<void()>& f : temp_functions) {
        f();
    }
}
void gearoenix::render::engine::Engine::add_load_function(std::function<void()> fun)
{
    load_functions_mutex->lock();
    load_functions.push_back(fun);
    load_functions_mutex->release();
}

const std::shared_ptr<gearoenix::render::scene::Scene>& gearoenix::render::engine::Engine::get_scene(core::Id scene_id) const
{
#ifdef GX_DEBUG_MODE
    std::map<core::Id, std::shared_ptr<gearoenix::render::scene::Scene>>::const_iterator f = loaded_scenes.find(scene_id);
    if (f == loaded_scenes.end())
        GXUNEXPECTED;
    return f->second;
#else
    return loaded_scenes[scene_id];
#endif
}

const std::map<gearoenix::core::Id, std::shared_ptr<gearoenix::render::scene::Scene>>& gearoenix::render::engine::Engine::get_all_scenes() const
{
    return loaded_scenes;
}

void gearoenix::render::engine::Engine::load_scene(core::Id scene_id, std::function<void()> on_load)
{
    scene_loader_mutex->lock();
    scene_loader_functions.push_back([this, scene_id, on_load] {
        loaded_scenes_mutex->lock();
#ifdef GX_DEBUG_MODE
        if (loaded_scenes.find(scene_id) != loaded_scenes.end())
            GXUNEXPECTED;
#endif
        sysapp->get_asset_manager()->get_scene_manager()->get(
            scene_id, core::sync::EndCaller<scene::Scene>([this, on_load, scene_id](std::shared_ptr<scene::Scene> asset) -> void {
                loaded_scenes[scene_id] = asset;
                asset->set_renderable(true);
                on_load();
            }));
        loaded_scenes_mutex->release();
    });
    scene_loader_mutex->release();
    scene_loader_signaler->release();
}

void gearoenix::render::engine::Engine::delete_scene(core::Id scene_id)
{
    loaded_scenes_mutex->lock();
    auto search = loaded_scenes.find(scene_id);
#ifdef GX_DEBUG_MODE
    if (loaded_scenes.end() == search)
        GXUNEXPECTED;
#endif
    loaded_scenes.erase(search);
    loaded_scenes_mutex->release();
}

gearoenix::physics::Engine* gearoenix::render::engine::Engine::get_physics_engine()
{
    return physics_engine;
}
