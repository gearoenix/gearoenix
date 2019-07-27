#include "rnd-eng-engine.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-function-loader.hpp"
#include "../../core/cr-static.hpp"
#include "../../core/event/cr-ev-event.hpp"
#include "../../core/event/cr-ev-sys-system.hpp"
#include "../../core/sync/cr-sync-kernel-workers.hpp"
#include "../../core/sync/cr-sync-semaphore.hpp"
#include "../../physics/phs-engine.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-log.hpp"
#include "../buffer/rnd-buf-manager.hpp"
#include "../command/rnd-cmd-manager.hpp"
#include "../graph/tree/rnd-gr-tr-pbr.hpp"
#include "../pipeline/rnd-pip-manager.hpp"
#include "../scene/rnd-scn-manager.hpp"
#include "../scene/rnd-scn-scene.hpp"
#include <thread>

gearoenix::render::engine::Engine::Engine(system::Application* const system_application, const Type::Id engine_type_id) noexcept
    : engine_type_id(engine_type_id)
    , sys_app(system_application)
    , fun_loader(new core::FunctionLoader())
    , kernels(new core::sync::KernelWorker())
{
    physics_engine = new physics::Engine(system_application, kernels);
    kernels->add_step(
        [this](const unsigned int kernel_index) {
            render_tree->record(kernel_index);
        },
        [this] {
            fun_loader->unload();
            render_tree->submit();
        });
}

gearoenix::render::engine::Engine::~Engine() noexcept
{
    terminate();
}

void gearoenix::render::engine::Engine::update() noexcept
{
    const std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<core::Real> delta_time_duration = now - last_frame_time;
    delta_time = delta_time_duration.count();
    last_frame_time = now;
    /// TODO: When Vulkan implemented take care of frame_node and do the frame synchronization by it.
    ++frame_number;
    frame_number %= frames_count;
    fun_loader->unload();
    physics_engine->update();
    fun_loader->unload();
    render_tree->update();
    fun_loader->unload();
    kernels->do_steps();
}

void gearoenix::render::engine::Engine::terminate() noexcept
{
    GX_DELETE(physics_engine)
    GX_DELETE(kernels)
    GX_DELETE(render_tree)
    main_render_target = nullptr;
    frame_node = nullptr;
    GX_DELETE(pipeline_manager)
    GX_DELETE(command_manager)
    GX_DELETE(sampler_manager)
    GX_DELETE(buffer_manager)
    if (fun_loader != nullptr) {
        fun_loader->unload();
        delete fun_loader;
        fun_loader = nullptr;
    }
}

void gearoenix::render::engine::Engine::set_render_tree(graph::tree::Tree* const tree) noexcept
{
    render_tree = tree;
}

const gearoenix::system::Application* gearoenix::render::engine::Engine::get_system_application() const noexcept
{
    return sys_app;
}

gearoenix::system::Application* gearoenix::render::engine::Engine::get_system_application() noexcept
{
    return sys_app;
}

const gearoenix::physics::Engine* gearoenix::render::engine::Engine::get_physics_engine() const noexcept
{
    return physics_engine;
}

gearoenix::physics::Engine* gearoenix::render::engine::Engine::get_physics_engine() noexcept
{
    return physics_engine;
}

const gearoenix::render::pipeline::Manager* gearoenix::render::engine::Engine::get_pipeline_manager() const noexcept
{
    return pipeline_manager;
}

gearoenix::render::pipeline::Manager* gearoenix::render::engine::Engine::get_pipeline_manager() noexcept
{
    return pipeline_manager;
}

const gearoenix::render::command::Manager* gearoenix::render::engine::Engine::get_command_manager() const noexcept
{
    return command_manager;
}

gearoenix::render::command::Manager* gearoenix::render::engine::Engine::get_command_manager() noexcept
{
    return command_manager;
}

const gearoenix::render::buffer::Manager* gearoenix::render::engine::Engine::get_buffer_manager() const noexcept
{
    return buffer_manager;
}

gearoenix::render::buffer::Manager* gearoenix::render::engine::Engine::get_buffer_manager() noexcept
{
    return buffer_manager;
}

const gearoenix::core::sync::KernelWorker* gearoenix::render::engine::Engine::get_kernels() const noexcept
{
    return kernels;
}

gearoenix::core::sync::KernelWorker* gearoenix::render::engine::Engine::get_kernels() noexcept
{
    return kernels;
}

const gearoenix::core::FunctionLoader* gearoenix::render::engine::Engine::get_function_loader() const noexcept
{
    return fun_loader;
}

gearoenix::core::FunctionLoader* gearoenix::render::engine::Engine::get_function_loader() noexcept
{
    return fun_loader;
}

gearoenix::render::engine::Type::Id gearoenix::render::engine::Engine::get_engine_type_id() const noexcept
{
    return engine_type_id;
}

const std::shared_ptr<gearoenix::render::texture::Target>& gearoenix::render::engine::Engine::get_main_render_target() const noexcept
{
    return main_render_target;
}

unsigned int gearoenix::render::engine::Engine::get_frame_number() const noexcept
{
    return frame_number;
}

unsigned int gearoenix::render::engine::Engine::get_frames_count() const noexcept
{
    return frames_count;
}

gearoenix::core::Real gearoenix::render::engine::Engine::get_delta_time() const noexcept
{
    return delta_time;
}
