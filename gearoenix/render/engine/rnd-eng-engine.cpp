#include "rnd-eng-engine.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/event/cr-ev-event.hpp"
#include "../../core/event/cr-ev-sys-system.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../core/sync/cr-sync-queued-semaphore.hpp"
#include "../../physics/phs-engine.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-log.hpp"
#include "../pipeline/rnd-pip-manager.hpp"
#include "../scene/rnd-scn-manager.hpp"
#include "../scene/rnd-scn-scene.hpp"
#include <functional>
#include <thread>

gearoenix::render::engine::Engine::Engine(const std::shared_ptr<system::Application>& system_application, const Type::Id engine_type_id)
    : engine_type_id(engine_type_id)
    , sysapp(system_application)
{
    //physics_engine = new physics::Engine(this);
}

gearoenix::render::engine::Engine::~Engine()
{
}

void gearoenix::render::engine::Engine::update() {
	++frame_number;
	frame_number %= GX_FRAMES_COUNT;
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

const std::shared_ptr<gearoenix::system::Application>& gearoenix::render::engine::Engine::get_system_application() const
{
	return sysapp;
}

const std::shared_ptr<gearoenix::physics::Engine>& gearoenix::render::engine::Engine::get_physics_engine() const
{
	GXUNIMPLEMENTED;
    return nullptr;
}

const std::shared_ptr<gearoenix::render::pipeline::Manager> &gearoenix::render::engine::Engine::get_pipeline_manager() const {
	return pipeline_manager;
}

const std::shared_ptr<gearoenix::render::command::Manager> &gearoenix::render::engine::Engine::get_command_manager() const {
	return command_manager;
}

const std::shared_ptr<gearoenix::render::buffer::Manager>& gearoenix::render::engine::Engine::get_buffer_manager() const
{
	return buffer_manager;
}

gearoenix::render::engine::Type::Id gearoenix::render::engine::Engine::get_engine_type_id() const
{
	return engine_type_id;
}

unsigned int gearoenix::render::engine::Engine::get_frame_number() const
{
	return frame_number;
}
