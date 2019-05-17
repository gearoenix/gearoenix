#include "rnd-eng-engine.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-function-loader.hpp"
#include "../../core/event/cr-ev-event.hpp"
#include "../../core/event/cr-ev-sys-system.hpp"
#include "../../core/sync/cr-sync-kernel-workers.hpp"
#include "../../core/sync/cr-sync-semaphore.hpp"
#include "../../physics/phs-engine.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-log.hpp"
#include "../graph/tree/rnd-gr-tr-pbr.hpp"
#include "../pipeline/rnd-pip-manager.hpp"
#include "../scene/rnd-scn-manager.hpp"
#include "../scene/rnd-scn-scene.hpp"
#include <thread>

gearoenix::render::engine::Engine::Engine(const std::shared_ptr<system::Application>& system_application, const Type::Id engine_type_id)
    : engine_type_id(engine_type_id)
    , sysapp(system_application)
    , fun_loader(new core::FunctionLoader())
    , kernels(new core::sync::KernelWorker())
{
    physics_engine = std::make_shared<physics::Engine>(system_application, kernels);
    kernels->add_step([this](const unsigned int kernel_index) { render_tree->record(kernel_index); }, [this] { render_tree->submit(); });
}

gearoenix::render::engine::Engine::~Engine()
{
    terminate();
}

void gearoenix::render::engine::Engine::update()
{
    const std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<core::Real> delta_time_duration = now - last_frame_time;
    delta_time = delta_time_duration.count();
    last_frame_time = now;

    ++frame_number;
    frame_number %= frames_count;
    physics_engine->update();
    fun_loader->unload();
    render_tree->update();
    kernels->do_steps();
}

void gearoenix::render::engine::Engine::terminate()
{
    sysapp = nullptr;
    physics_engine = nullptr;
    kernels = nullptr;
    render_tree = nullptr;
    main_render_target = nullptr;
    pipeline_manager = nullptr;
    command_manager = nullptr;
    sampler_manager = nullptr;
    buffer_manager = nullptr;

    if (fun_loader != nullptr)
        fun_loader->unload();
    fun_loader = nullptr;
}

void gearoenix::render::engine::Engine::set_render_tree(const std::shared_ptr<graph::tree::Tree>& tree)
{
    render_tree = tree;
}

const std::shared_ptr<gearoenix::system::Application>& gearoenix::render::engine::Engine::get_system_application() const
{
    return sysapp;
}

const std::shared_ptr<gearoenix::physics::Engine>& gearoenix::render::engine::Engine::get_physics_engine() const
{
    return physics_engine;
}

const std::shared_ptr<gearoenix::render::pipeline::Manager>& gearoenix::render::engine::Engine::get_pipeline_manager() const
{
    return pipeline_manager;
}

const std::shared_ptr<gearoenix::render::command::Manager>& gearoenix::render::engine::Engine::get_command_manager() const
{
    return command_manager;
}

const std::shared_ptr<gearoenix::render::buffer::Manager>& gearoenix::render::engine::Engine::get_buffer_manager() const
{
    return buffer_manager;
}

const std::shared_ptr<gearoenix::core::sync::KernelWorker>& gearoenix::render::engine::Engine::get_kernels() const
{
    return kernels;
}

const std::shared_ptr<gearoenix::core::FunctionLoader>& gearoenix::render::engine::Engine::get_function_loader() const
{
    return fun_loader;
}

gearoenix::render::engine::Type::Id gearoenix::render::engine::Engine::get_engine_type_id() const
{
    return engine_type_id;
}

const std::shared_ptr<gearoenix::render::texture::Target>& gearoenix::render::engine::Engine::get_main_render_target() const
{
    return main_render_target;
}

unsigned int gearoenix::render::engine::Engine::get_frame_number() const
{
    return frame_number;
}

unsigned int gearoenix::render::engine::Engine::get_frames_count() const
{
    return frames_count;
}

gearoenix::core::Real gearoenix::render::engine::Engine::get_delta_time() const noexcept
{
    return delta_time;
}
