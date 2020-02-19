#include "rnd-gr-nd-skybox-equirectangular.hpp"
#include "../../../core/asset/cr-asset-manager.hpp"
#include "../../../core/sync/cr-sync-kernel-workers.hpp"
#include "../../../system/sys-app.hpp"
#include "../../buffer/rnd-buf-manager.hpp"
#include "../../buffer/rnd-buf-uniform.hpp"
#include "../../camera/rnd-cmr-camera.hpp"
#include "../../command/rnd-cmd-buffer.hpp"
#include "../../command/rnd-cmd-manager.hpp"
#include "../../engine/rnd-eng-engine.hpp"
#include "../../material/rnd-mat-material.hpp"
#include "../../mesh/rnd-msh-mesh.hpp"
#include "../../model/rnd-mdl-mesh.hpp"
#include "../../model/rnd-mdl-model.hpp"
#include "../../pipeline/rnd-pip-manager.hpp"
#include "../../pipeline/rnd-pip-pipeline.hpp"
#include "../../pipeline/rnd-pip-skybox-equirectangular-resource-set.hpp"
#include "../../skybox/rnd-sky-equirectangular.hpp"
#include "../../sync/rnd-sy-semaphore.hpp"
#include "../../texture/rnd-txt-manager.hpp"
#include "../../texture/rnd-txt-texture-cube.hpp"

gearoenix::render::graph::node::SkyboxEquirectangularRenderData::SkyboxEquirectangularRenderData(engine::Engine* e, pipeline::Pipeline* pip) noexcept
    : r(dynamic_cast<pipeline::SkyboxEquirectangularResourceSet*>(pip->create_resource_set()))
    , u(e->get_buffer_manager()->create_uniform(sizeof(SkyboxEquirectangularUniform)))
{
    r->set_node_uniform_buffer(u.get());
}

gearoenix::render::graph::node::SkyboxEquirectangularRenderData::~SkyboxEquirectangularRenderData() noexcept = default;

gearoenix::render::graph::node::SkyboxEquirectangularKernel::SkyboxEquirectangularKernel(engine::Engine* e, const unsigned int kernel_index) noexcept
    : secondary_cmd(e->get_command_manager()->create_secondary_command_buffer(kernel_index))
{
}

gearoenix::render::graph::node::SkyboxEquirectangularKernel::~SkyboxEquirectangularKernel() noexcept = default;

gearoenix::render::graph::node::SkyboxEquirectangularFrame::SkyboxEquirectangularFrame(gearoenix::render::engine::Engine* e) noexcept
    : kernels(e->get_kernels()->get_threads_count())
{
    for (std::size_t i = 0; i < kernels.size(); ++i) {
        kernels[i] = std::make_unique<SkyboxEquirectangularKernel>(e, static_cast<unsigned int>(i));
    }
}

gearoenix::render::graph::node::SkyboxEquirectangularFrame::~SkyboxEquirectangularFrame() noexcept = default;

void gearoenix::render::graph::node::SkyboxEquirectangular::record_sky(
    const skybox::Equirectangular* const sky,
    const SkyboxEquirectangularUniform& u,
    SkyboxEquirectangularKernel* const kernel) noexcept
{
    auto* const rd = kernel->render_data_pool.get_next([this] {
        return new SkyboxEquirectangularRenderData(e, render_pipeline.get());
    });
    rd->u->set_data(u);
    auto* const prs = rd->r.get();
    prs->set_mesh(sky->get_msh().get());
    prs->set_material(sky->get_mat_equ().get());
    kernel->secondary_cmd->bind(prs);
}

gearoenix::render::graph::node::SkyboxEquirectangular::SkyboxEquirectangular(
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
    : Node(
        Type::SkyboxEquirectangular,
        e,
        pipeline::Type::SkyboxEquirectangular,
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
        f = std::make_unique<SkyboxEquirectangularFrame>(e);
    }
}

gearoenix::render::graph::node::SkyboxEquirectangular::~SkyboxEquirectangular() noexcept = default;

void gearoenix::render::graph::node::SkyboxEquirectangular::update() noexcept
{
    Node::update();
    const unsigned int frame_number = e->get_frame_number();
    frame = frames[frame_number].get();
    auto& kernels = frame->kernels;
    for (auto& kernel : kernels) {
        kernel->render_data_pool.refresh();
        kernel->secondary_cmd->begin();
    }
    skies.clear();
}

void gearoenix::render::graph::node::SkyboxEquirectangular::set_camera(const camera::Camera* const c) noexcept
{
    cam = c;
    render_target = c->get_target();
}

void gearoenix::render::graph::node::SkyboxEquirectangular::add_sky(const skybox::Equirectangular* const sky) noexcept
{
    skies.push_back(sky);
}

void gearoenix::render::graph::node::SkyboxEquirectangular::record_continuously(const unsigned int kernel_index) noexcept
{
    auto* const kernel = frame->kernels[kernel_index].get();
    for (const auto* const sky : skies) {
        const SkyboxEquirectangularUniform u {
            .mvp = cam->get_uniform().view_projection * math::Mat4x4(std::abs(cam->get_uniform().far) * 0.57735026f),
        };
        record_sky(sky, u, kernel);
    }
}

void gearoenix::render::graph::node::SkyboxEquirectangular::submit() noexcept
{
    const unsigned int frame_number = e->get_frame_number();
    command::Buffer* cmd = frames_primary_cmd[frame_number].get();
    cmd->bind(render_target);
    for (const auto& k : frame->kernels) {
        cmd->record(k->secondary_cmd.get());
    }
    Node::submit();
}
