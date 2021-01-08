#include "gx-rnd-gr-nd-radiance-convoluter.hpp"
#include "../../../core/asset/gx-cr-asset-manager.hpp"
#include "../../../core/sync/gx-cr-sync-kernel-workers.hpp"
#include "../../../physics/collider/gx-phs-cld-collider.hpp"
#include "../../../platform/gx-plt-application.hpp"
#include "../../buffer/gx-rnd-buf-manager.hpp"
#include "../../buffer/gx-rnd-buf-uniform.hpp"
#include "../../camera/gx-rnd-cmr-camera.hpp"
#include "../../command/gx-rnd-cmd-buffer.hpp"
#include "../../command/gx-rnd-cmd-manager.hpp"
#include "../../engine/gx-rnd-eng-engine.hpp"
#include "../../mesh/gx-rnd-msh-mesh.hpp"
#include "../../pipeline/gx-rnd-pip-manager.hpp"
#include "../../pipeline/gx-rnd-pip-pipeline.hpp"
#include "../../pipeline/gx-rnd-pip-radiance-convoluter-resource-set.hpp"
#include "../../sync/gx-rnd-sy-semaphore.hpp"
#include "../../texture/gx-rnd-txt-manager.hpp"
#include "../../texture/gx-rnd-txt-target.hpp"
#include "../../texture/gx-rnd-txt-texture-cube.hpp"

gearoenix::render::graph::node::RadianceConvoluterUniform::RadianceConvoluterUniform(const float r, const float resolution) noexcept
    : roughness(r)
    , roughness_p_4(r * r * r * r)
    , sa_texel(static_cast<float>(GX_PI * 4.0f / (6.0f * resolution * resolution)))
{
}

gearoenix::render::graph::node::RadianceConvoluterKernel::RadianceConvoluterKernel(
    const float roughness,
    const float resolution,
    engine::Engine* const e,
    pipeline::Pipeline* const pip,
    const unsigned int kernel_index) noexcept
    : secondary_cmd(e->get_command_manager()->create_secondary_command_buffer(kernel_index))
    , r(dynamic_cast<pipeline::RadianceConvoluterResourceSet*>(pip->create_resource_set()))
    , u(e->get_buffer_manager()->create_uniform(sizeof(RadianceConvoluterUniform), 0))
{
    RadianceConvoluterUniform ud(roughness, resolution);
    u->set_data(ud);
    r->set_node_uniform_buffer(u.get());
}

gearoenix::render::graph::node::RadianceConvoluterKernel::~RadianceConvoluterKernel() noexcept = default;

gearoenix::render::graph::node::RadianceConvoluterFrame::RadianceConvoluterFrame(
    const float roughness,
    const float resolution,
    engine::Engine* const e,
    pipeline::Pipeline* const pip) noexcept
    : kernels(e->get_kernels()->get_threads_count())
{
    for (std::size_t i = 0; i < kernels.size(); ++i) {
        kernels[i] = std::make_unique<RadianceConvoluterKernel>(roughness, resolution, e, pip, static_cast<unsigned int>(i));
    }
}

gearoenix::render::graph::node::RadianceConvoluterFrame::~RadianceConvoluterFrame() noexcept = default;

void gearoenix::render::graph::node::RadianceConvoluter::record(RadianceConvoluterKernel* const kernel) noexcept
{
    auto* const prs = kernel->r.get();
    prs->set_mesh(msh);
    prs->set_environment(environment);
    kernel->secondary_cmd->bind(prs);
}

gearoenix::render::graph::node::RadianceConvoluter::RadianceConvoluter(
    std::string name,
    const float roughness,
    const float resolution,
    const mesh::Mesh* const msh,
    const texture::TextureCube* const environment,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
    : Node(
        std::move(name),
        Type::RadianceConvoluter,
        e,
        pipeline::Type::RadianceConvoluter,
        0,
        1,
        {},
        {
            "color",
        },
        call)
    , frames(e->get_frames_count())
    , msh(msh)
    , environment(environment)
{
    set_providers_count(input_textures.size());
    for (auto& f : frames) {
        f = std::make_unique<RadianceConvoluterFrame>(roughness, resolution, e, render_pipeline.get());
    }
}

gearoenix::render::graph::node::RadianceConvoluter::~RadianceConvoluter() noexcept = default;

void gearoenix::render::graph::node::RadianceConvoluter::update() noexcept
{
    Node::update();
    const auto frame_number = e->get_frame_number();
    frame = frames[frame_number].get();
    auto& kernels = frame->kernels;
    for (auto& kernel : kernels) {
        kernel->secondary_cmd->begin();
    }
}

void gearoenix::render::graph::node::RadianceConvoluter::record(const unsigned int) noexcept
{
    GX_UNEXPECTED
}

void gearoenix::render::graph::node::RadianceConvoluter::record_continuously(const unsigned int kernel_index) noexcept
{
    auto* const kernel = frame->kernels[kernel_index].get();
    record(kernel);
}

void gearoenix::render::graph::node::RadianceConvoluter::submit() noexcept
{
    const unsigned int frame_number = e->get_frame_number();
    command::Buffer* cmd = frames_primary_cmd[frame_number].get();
    cmd->bind(render_target);
    for (const auto& k : frame->kernels) {
        cmd->record(k->secondary_cmd.get());
    }
    Node::submit();
}
