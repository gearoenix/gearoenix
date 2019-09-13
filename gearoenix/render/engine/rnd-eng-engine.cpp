#include "rnd-eng-engine.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-function-loader.hpp"
#include "../../core/cr-static.hpp"
#include "../../core/event/cr-ev-event.hpp"
#include "../../core/sync/cr-sync-kernel-workers.hpp"
#include "../../core/sync/cr-sync-semaphore.hpp"
#include "../../physics/phs-engine.hpp"
#include "../../system/sys-app.hpp"
#include "../buffer/rnd-buf-manager.hpp"
#include "../command/rnd-cmd-manager.hpp"
#include "../graph/tree/rnd-gr-tr-pbr.hpp"
#include "../pipeline/rnd-pip-manager.hpp"
#include "../sampler/rnd-smp-manager.hpp"
#include "../scene/rnd-scn-manager.hpp"
#include "../scene/rnd-scn-scene.hpp"
#include <thread>

gearoenix::render::engine::Engine::Engine(system::Application* const system_application, const Type engine_type) noexcept
    : engine_type(engine_type)
    , system_application(system_application)
    , function_loader(new core::FunctionLoader())
    , kernels(new core::sync::KernelWorker())
    , physics_engine(new physics::Engine(system_application, kernels.get()))
{
    kernels->add_step(
        [this] {
            function_loader->unload();
            render_tree->update();
            function_loader->unload();
        },
        [this](const unsigned int kernel_index) {
            render_tree->record(kernel_index);
        },
        [this] {
            function_loader->unload();
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
    function_loader->unload();
    physics_engine->update();
    function_loader->unload();
    kernels->do_steps();
}

void gearoenix::render::engine::Engine::terminate() noexcept
{
    if (function_loader == nullptr)
        return;
    buffer_manager = nullptr;
    sampler_manager = nullptr;
    command_manager = nullptr;
    pipeline_manager = nullptr;
    physics_engine = nullptr;
    main_render_target = nullptr;
    kernels = nullptr;
    function_loader->unload();
    function_loader = nullptr;
}