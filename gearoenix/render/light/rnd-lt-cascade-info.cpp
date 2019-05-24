#include "rnd-lt-cascade-info.hpp"
#include "../../core/sync/cr-sync-kernel-workers.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-configuration.hpp"
#include "../buffer/rnd-buf-uniform.hpp"
#include "../command/rnd-cmd-buffer.hpp"
#include "../command/rnd-cmd-manager.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../sync/rnd-sy-semaphore.hpp"

gearoenix::render::light::MathCascadeInfo::MathCascadeInfo(engine::Engine* e) noexcept
    : cameras(e->get_system_application()->get_configuration().render_config.shadow_cascades_count)
{
}

void gearoenix::render::light::MathCascadeInfo::initialize(engine::Engine* const e) noexcept
{
    cameras.resize(e->get_system_application()->get_configuration().render_config.shadow_cascades_count);
}

gearoenix::render::light::KernelCascadeInfo::KernelRenderCascadeInfo::~KernelRenderCascadeInfo() noexcept
{
    delete uniform;
    uniform = nullptr;
}

gearoenix::render::light::KernelCascadeInfo::KernelCascadeInfo(engine::Engine* const e, const std::size_t kernel_index) noexcept
    : kernel_index(kernel_index)
    , math_info(e)
    , shadow_mapper_secondary_commands(static_cast<std::size_t>(e->get_system_application()->get_configuration().render_config.shadow_cascades_count))
{
    command::Manager* cmdmgr = e->get_command_manager();
    for (command::Buffer*& b : shadow_mapper_secondary_commands)
        b = cmdmgr->create_secondary_command_buffer(kernel_index);
}

gearoenix::render::light::KernelCascadeInfo::~KernelCascadeInfo() noexcept
{
    for (command::Buffer* b : shadow_mapper_secondary_commands)
        delete b;
    shadow_mapper_secondary_commands.clear();
    render_data_pool.clear();
    pool_last_index = 0;
}

gearoenix::render::light::FrameCascadeInfo::FrameCascadeInfo(engine::Engine* const e) noexcept
    : kernels(static_cast<std::size_t>(e->get_kernels()->get_threads_count()))
    , math_info(e)
    , shadow_mapper_primary_commands(static_cast<std::size_t>(e->get_system_application()->get_configuration().render_config.shadow_cascades_count))
    , shadow_mappers_semaphore(e->create_semaphore())
    , shadow_accumulator_secondary_command(e->get_command_manager()->create_secondary_command_buffer())
    , shadow_accumulator_primary_command(e->get_command_manager()->create_primary_command_buffer())
    , shadow_accumulator_semaphore(e->create_semaphore())
{
    for (std::size_t i = 0; i < kernels.size(); ++i)
        kernels[i] = new KernelCascadeInfo(e, i);
    command::Manager* cmdmgr = e->get_command_manager();
    for (command::Buffer*& b : shadow_mapper_primary_commands)
        b = cmdmgr->create_primary_command_buffer();
}

gearoenix::render::light::FrameCascadeInfo::~FrameCascadeInfo() noexcept
{
    for (KernelCascadeInfo* k : kernels)
        delete k;
    kernels.clear();
    for (command::Buffer* b : shadow_mapper_primary_commands)
        delete b;
    shadow_mapper_primary_commands.clear();
    delete shadow_mappers_semaphore;
    shadow_mappers_semaphore = nullptr;
    delete shadow_accumulator_secondary_command;
    shadow_accumulator_secondary_command = nullptr;
    delete shadow_accumulator_primary_command;
    shadow_accumulator_primary_command = nullptr;
    delete shadow_accumulator_semaphore;
    shadow_accumulator_semaphore = nullptr;
}

gearoenix::render::light::CascadeInfo::CascadeInfo(engine::Engine* const e) noexcept
    : e(e)
{
    const auto frames_count = static_cast<std::size_t>(e->get_frames_count());
    frames.resize(frames_count);
    for (FrameCascadeInfo*& frame : frames)
        frame = new FrameCascadeInfo(e);
}

gearoenix::render::light::CascadeInfo::~CascadeInfo() noexcept
{
    for (FrameCascadeInfo* frame : frames)
        delete frame;
    frames.clear();
}
