#include "rnd-gr-nd-mipmap-generator.hpp"
#include "../../../core/asset/cr-asset-manager.hpp"
#include "../../../core/sync/cr-sync-kernel-workers.hpp"
#include "../../../system/sys-app.hpp"
#include "../../buffer/rnd-buf-manager.hpp"
#include "../../buffer/rnd-buf-uniform.hpp"
#include "../../command/rnd-cmd-buffer.hpp"
#include "../../command/rnd-cmd-manager.hpp"
#include "../../engine/rnd-eng-engine.hpp"
#include "../../pipeline/rnd-pip-irradiance-convoluter-resource-set.hpp"
#include "../../pipeline/rnd-pip-manager.hpp"
#include "../../pipeline/rnd-pip-pipeline.hpp"
#include "../../sync/rnd-sy-semaphore.hpp"
#include "../../texture/rnd-txt-manager.hpp"
#include "../../texture/rnd-txt-target.hpp"
#include "../../texture/rnd-txt-texture-cube.hpp"

gearoenix::render::graph::node::MipmapGeneratorKernel::MipmapGeneratorKernel(engine::Engine* e, const unsigned int kernel_index) noexcept
    : secondary_cmd(e->get_command_manager()->create_secondary_command_buffer(kernel_index))
{
}

gearoenix::render::graph::node::MipmapGeneratorKernel::~MipmapGeneratorKernel() noexcept = default;

gearoenix::render::graph::node::MipmapGeneratorFrame::MipmapGeneratorFrame(engine::Engine* const e) noexcept
    : kernels(e->get_kernels()->get_threads_count())
{
    for (std::size_t i = 0; i < kernels.size(); ++i) {
        kernels[i] = std::make_unique<MipmapGeneratorKernel>(e, static_cast<unsigned int>(i));
    }
}

gearoenix::render::graph::node::MipmapGeneratorFrame::~MipmapGeneratorFrame() noexcept = default;

void gearoenix::render::graph::node::MipmapGenerator::record(MipmapGeneratorKernel* const kernel) noexcept
{
    kernel->secondary_cmd->record_generate_mipmap(base_texture);
}

gearoenix::render::graph::node::MipmapGenerator::MipmapGenerator(
    std::string name,
    texture::Texture* const base_texture,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
    : Node(
        std::move(name),
        Type::MipmapGenerator,
        e,
        pipeline::Type::Unlit, // this is just for filling
        0,
        1,
        {},
        {
            "color",
        },
        call)
    , frames(e->get_frames_count())
    , base_texture(base_texture)
{
    set_providers_count(input_textures.size());
    for (auto& f : frames) {
        f = std::make_unique<MipmapGeneratorFrame>(e);
    }
}

gearoenix::render::graph::node::MipmapGenerator::~MipmapGenerator() noexcept = default;

void gearoenix::render::graph::node::MipmapGenerator::update() noexcept
{
    Node::update();
    const auto frame_number = e->get_frame_number();
    frame = frames[frame_number].get();
    auto& kernels = frame->kernels;
    for (auto& kernel : kernels) {
        kernel->secondary_cmd->begin();
    }
}

void gearoenix::render::graph::node::MipmapGenerator::record(const unsigned int) noexcept
{
    GX_UNEXPECTED
}

void gearoenix::render::graph::node::MipmapGenerator::record_continuously(const unsigned int kernel_index) noexcept
{
    auto* const kernel = frame->kernels[kernel_index].get();
    record(kernel);
}

void gearoenix::render::graph::node::MipmapGenerator::submit() noexcept
{
    const unsigned int frame_number = e->get_frame_number();
    command::Buffer* cmd = frames_primary_cmd[frame_number].get();
    cmd->bind(render_target);
    for (const auto& k : frame->kernels) {
        cmd->record(k->secondary_cmd.get());
    }
    Node::submit();
}
