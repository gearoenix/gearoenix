#include "rnd-eng-engine.hpp"
#include "../../core/cr-function-loader.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/event/cr-ev-event.hpp"
#include "../../core/event/cr-ev-sys-system.hpp"
#include "../../core/sync/cr-sync-kernel-workers.hpp"
#include "../../core/sync/cr-sync-queued-semaphore.hpp"
#include "../../physics/phs-engine.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-log.hpp"
#include "../graph/tree/rnd-gr-tr-pbr.hpp"
#include "../pipeline/rnd-pip-manager.hpp"
#include "../scene/rnd-scn-manager.hpp"
#include "../scene/rnd-scn-scene.hpp"
#include <thread>

void gearoenix::render::engine::Engine::record(const unsigned int kernel_index)
{
	render_tree->record(kernel_index);
}

gearoenix::render::engine::Engine::Engine(const std::shared_ptr<system::Application>& system_application, const Type::Id engine_type_id)
    : engine_type_id(engine_type_id)
    , sysapp(system_application)
	, fun_loader(new core::FunctionLoader())
    , physics_engine(nullptr)
	, kernels(new core::sync::KernelWorker())
{
	kernels->add_step(std::bind(&Engine::record, this, std::placeholders::_1));
}

gearoenix::render::engine::Engine::~Engine()
{
	sysapp = nullptr;
	fun_loader = nullptr;
	physics_engine = nullptr;
	kernels = nullptr;
	pipeline_manager = nullptr;
	command_manager = nullptr;
	sampler_manager = nullptr;
	buffer_manager = nullptr;
}

void gearoenix::render::engine::Engine::update() {
	++frame_number;
	frame_number %= frames_count;
	kernels->do_steps();
}

const std::shared_ptr<gearoenix::system::Application>& gearoenix::render::engine::Engine::get_system_application() const
{
	return sysapp;
}

const std::shared_ptr<gearoenix::physics::Engine>& gearoenix::render::engine::Engine::get_physics_engine() const
{
    return physics_engine;
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

const std::shared_ptr<gearoenix::core::FunctionLoader>& gearoenix::render::engine::Engine::get_function_loader() const
{
	return fun_loader;
}

gearoenix::render::engine::Type::Id gearoenix::render::engine::Engine::get_engine_type_id() const
{
	return engine_type_id;
}

unsigned int gearoenix::render::engine::Engine::get_frame_number() const
{
	return frame_number;
}

unsigned int gearoenix::render::engine::Engine::get_frames_count() const
{
	return frames_count;
}
