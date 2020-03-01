#include "rnd-gr-nd-irradiance-convoluter.hpp"
#include "../../../core/asset/cr-asset-manager.hpp"
#include "../../../core/sync/cr-sync-kernel-workers.hpp"
#include "../../../physics/collider/phs-cld-collider.hpp"
#include "../../../system/sys-app.hpp"
#include "../../buffer/rnd-buf-manager.hpp"
#include "../../buffer/rnd-buf-uniform.hpp"
#include "../../camera/rnd-cmr-camera.hpp"
#include "../../command/rnd-cmd-buffer.hpp"
#include "../../command/rnd-cmd-manager.hpp"
#include "../../engine/rnd-eng-engine.hpp"
#include "../../mesh/rnd-msh-mesh.hpp"
#include "../../pipeline/rnd-pip-irradiance-convoluter-resource-set.hpp"
#include "../../pipeline/rnd-pip-manager.hpp"
#include "../../pipeline/rnd-pip-pipeline.hpp"
#include "../../sync/rnd-sy-semaphore.hpp"
#include "../../texture/rnd-txt-manager.hpp"
#include "../../texture/rnd-txt-target.hpp"
#include "../../texture/rnd-txt-texture-cube.hpp"

gearoenix::render::graph::node::IrradianceConvoluterRenderData::IrradianceConvoluterRenderData(engine::Engine* e, pipeline::Pipeline* pip) noexcept
    : r(dynamic_cast<pipeline::IrradianceConvoluterResourceSet*>(pip->create_resource_set()))
{
}

gearoenix::render::graph::node::IrradianceConvoluterRenderData::~IrradianceConvoluterRenderData() noexcept = default;

gearoenix::render::graph::node::IrradianceConvoluterKernel::IrradianceConvoluterKernel(engine::Engine* e, const unsigned int kernel_index) noexcept
    : secondary_cmd(e->get_command_manager()->create_secondary_command_buffer(kernel_index))
{
}

gearoenix::render::graph::node::IrradianceConvoluterKernel::~IrradianceConvoluterKernel() noexcept = default;

gearoenix::render::graph::node::IrradianceConvoluterFrame::IrradianceConvoluterFrame(gearoenix::render::engine::Engine* e) noexcept
    : kernels(e->get_kernels()->get_threads_count())
{
    for (std::size_t i = 0; i < kernels.size(); ++i) {
        kernels[i] = std::make_unique<IrradianceConvoluterKernel>(e, static_cast<unsigned int>(i));
    }
}

gearoenix::render::graph::node::IrradianceConvoluterFrame::~IrradianceConvoluterFrame() noexcept = default;

void gearoenix::render::graph::node::IrradianceConvoluter::record(
    const mesh::Mesh* const msh,
    IrradianceConvoluterKernel* const kernel) noexcept
{
    auto* const rd = kernel->render_data_pool.get_next([this] {
        return new IrradianceConvoluterRenderData(e, render_pipeline.get());
    });
    auto* const prs = rd->r.get();
    prs->set_mesh(msh);
    kernel->secondary_cmd->bind(prs);
}

gearoenix::render::graph::node::IrradianceConvoluter::IrradianceConvoluter(
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
    : Node(
        Type::IrradianceConvoluter,
        e,
        pipeline::Type::IrradianceConvoluter,
        0,
        1,
        {},
        {
            "color",
        },
        call)
    , frames(e->get_frames_count())
{
    set_providers_count(input_textures.size());
    for (auto& f : frames) {
        f = std::make_unique<IrradianceConvoluterFrame>(e);
    }
}

gearoenix::render::graph::node::IrradianceConvoluter::~IrradianceConvoluter() noexcept = default;

void gearoenix::render::graph::node::IrradianceConvoluter::update() noexcept
{
    Node::update();
    const unsigned int frame_number = e->get_frame_number();
    frame = frames[frame_number].get();
    auto& kernels = frame->kernels;
    for (auto& kernel : kernels) {
        kernel->render_data_pool.refresh();
        kernel->secondary_cmd->begin();
    }
}

void gearoenix::render::graph::node::IrradianceConvoluter::record(const unsigned int) noexcept
{
    GXUNEXPECTED
}

void gearoenix::render::graph::node::IrradianceConvoluter::record_continuously(const unsigned int kernel_index) noexcept
{
    auto* const kernel = frame->kernels[kernel_index].get();
    for (const auto& msh : meshes) {
        record(msh.get(), kernel);
    }
}

void gearoenix::render::graph::node::IrradianceConvoluter::submit() noexcept
{
    const unsigned int frame_number = e->get_frame_number();
    command::Buffer* cmd = frames_primary_cmd[frame_number].get();
    cmd->bind(render_target);
    for (const auto& k : frame->kernels) {
        cmd->record(k->secondary_cmd.get());
    }
    Node::submit();
}
