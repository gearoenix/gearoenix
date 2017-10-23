#include "rnd-engine.hpp"
#include "../core/asset/cr-asset-manager.hpp"
#include "../core/cr-end-caller.hpp"
#include "../system/sys-app.hpp"
#include "pipeline/rnd-pip-manager.hpp"
#include "scene/rnd-scn-scene.hpp"

gearoenix::render::Engine::Engine(system::Application* system_application)
    : sysapp(system_application)
{
}

gearoenix::render::Engine::~Engine()
{
    delete pipmgr;
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

void gearoenix::render::Engine::add_load_function(std::function<void()> fun)
{
    std::lock_guard<std::mutex> lock(load_functions_mutex);
    load_functions.push_back(fun);
    (void)lock;
}

void gearoenix::render::Engine::load_scene(core::Id scene_id, std::function<void(unsigned int)> on_load)
{
    std::lock_guard<std::mutex> lock(loaded_scenes_mutex);
    unsigned int result = loaded_scenes.size();
    loaded_scenes.push_back(sysapp->get_asset_manager()->get_scene(scene_id, core::EndCaller::create([this, result, on_load] {
        loaded_scenes[result]->set_renderable(true);
        on_load(result);
    })));
    (void)lock;
}
