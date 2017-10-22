#include "rnd-engine.hpp"

gearoenix::render::Engine::Engine(system::Application* system_application)
    : sysapp(system_application)
{
}

gearoenix::render::Engine::~Engine() {}

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
