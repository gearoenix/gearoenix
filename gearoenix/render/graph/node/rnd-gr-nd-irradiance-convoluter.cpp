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
#include "../../mesh/rnd-msh-manager.hpp"
#include "../../mesh/rnd-msh-mesh.hpp"
#include "../../pipeline/rnd-pip-irradiance-convoluter-resource-set.hpp"
#include "../../pipeline/rnd-pip-manager.hpp"
#include "../../pipeline/rnd-pip-pipeline.hpp"
#include "../../sync/rnd-sy-semaphore.hpp"
#include "../../texture/rnd-txt-manager.hpp"
#include "../../texture/rnd-txt-target.hpp"
#include "../../texture/rnd-txt-texture-cube.hpp"

gearoenix::render::graph::node::IrradianceConvoluterKernel::IrradianceConvoluterKernel(engine::Engine* e, pipeline::Pipeline* const pip, const unsigned int kernel_index) noexcept
    : secondary_cmd(e->get_command_manager()->create_secondary_command_buffer(kernel_index))
    , r(dynamic_cast<pipeline::IrradianceConvoluterResourceSet*>(pip->create_resource_set()))
{
}

gearoenix::render::graph::node::IrradianceConvoluterKernel::~IrradianceConvoluterKernel() noexcept = default;

gearoenix::render::graph::node::IrradianceConvoluterFrame::IrradianceConvoluterFrame(engine::Engine* e, pipeline::Pipeline* pip) noexcept
    : kernels(e->get_kernels()->get_threads_count())
{
    for (std::size_t i = 0; i < kernels.size(); ++i) {
        kernels[i] = std::make_unique<IrradianceConvoluterKernel>(e, pip, static_cast<unsigned int>(i));
    }
}

gearoenix::render::graph::node::IrradianceConvoluterFrame::~IrradianceConvoluterFrame() noexcept = default;

void gearoenix::render::graph::node::IrradianceConvoluter::record(IrradianceConvoluterKernel* const kernel) noexcept
{
    auto* const prs = kernel->r.get();
    prs->set_mesh(cube_mesh.get());
    prs->set_environment(environment);
    prs->set_node_uniform_buffer(uniform.get());
    kernel->secondary_cmd->bind(prs);
}

gearoenix::render::graph::node::IrradianceConvoluter::IrradianceConvoluter(
    const math::Mat4x4<float>& mvp, const texture::TextureCube* environment,
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
    , uniform(e->get_buffer_manager()->create_uniform(sizeof(IrradianceConvoluterUniform)))
    , environment(environment)
{
    uniform->set_data(mvp);
    core::sync::EndCaller<mesh::Mesh> mesh_call([call](const std::shared_ptr<mesh::Mesh>&) {});
    cube_mesh = e->get_system_application()->get_asset_manager()->get_mesh_manager()->create_inward_cube(mesh_call);
    set_providers_count(input_textures.size());
    for (auto& f : frames) {
        f = std::make_unique<IrradianceConvoluterFrame>(e, render_pipeline.get());
    }
}

gearoenix::render::graph::node::IrradianceConvoluter::~IrradianceConvoluter() noexcept = default;

void gearoenix::render::graph::node::IrradianceConvoluter::update() noexcept
{
    Node::update();
    const auto frame_number = e->get_frame_number();
    frame = frames[frame_number].get();
    auto& kernels = frame->kernels;
    for (auto& kernel : kernels) {
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
    record(kernel);
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
