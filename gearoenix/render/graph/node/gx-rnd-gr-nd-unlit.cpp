#include "gx-rnd-gr-nd-unlit.hpp"
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
#include "../../light/gx-rnd-lt-point.hpp"
#include "../../material/gx-rnd-mat-material.hpp"
#include "../../mesh/gx-rnd-msh-mesh.hpp"
#include "../../model/gx-rnd-mdl-mesh.hpp"
#include "../../model/gx-rnd-mdl-model.hpp"
#include "../../pipeline/gx-rnd-pip-manager.hpp"
#include "../../pipeline/gx-rnd-pip-pipeline.hpp"
#include "../../pipeline/gx-rnd-pip-unlit-resource-set.hpp"
#include "../../scene/gx-rnd-scn-scene.hpp"
#include "../../sync/gx-rnd-sy-semaphore.hpp"
#include "../../texture/gx-rnd-txt-manager.hpp"
#include "../../texture/gx-rnd-txt-target.hpp"
#include "../../texture/gx-rnd-txt-texture-cube.hpp"

gearoenix::render::graph::node::UnlitRenderData::UnlitRenderData(
    engine::Engine* const e,
    pipeline::Pipeline* const pip,
    const std::size_t frame_number) noexcept
    : r(dynamic_cast<pipeline::UnlitResourceSet*>(pip->create_resource_set()))
    , u(e->get_buffer_manager()->create_uniform(sizeof(UnlitUniform), frame_number))
{
    r->set_node_uniform_buffer(u.get());
}

gearoenix::render::graph::node::UnlitRenderData::~UnlitRenderData() noexcept = default;

gearoenix::render::graph::node::UnlitKernel::UnlitKernel(engine::Engine* e, const unsigned int kernel_index) noexcept
    : secondary_cmd(e->get_command_manager()->create_secondary_command_buffer(kernel_index))
{
}

gearoenix::render::graph::node::UnlitKernel::~UnlitKernel() noexcept = default;

gearoenix::render::graph::node::UnlitFrame::UnlitFrame(gearoenix::render::engine::Engine* e) noexcept
    : kernels(e->get_kernels()->get_threads_count())
{
    for (std::size_t i = 0; i < kernels.size(); ++i) {
        kernels[i] = std::make_unique<UnlitKernel>(e, static_cast<unsigned int>(i));
    }
}

gearoenix::render::graph::node::UnlitFrame::~UnlitFrame() noexcept = default;

void gearoenix::render::graph::node::Unlit::record(
    const model::Mesh* const msh,
    const UnlitUniform& u,
    UnlitKernel* const kernel) noexcept
{
    auto* const rd = kernel->render_data_pool.get_next([this] {
        return new UnlitRenderData(e, render_pipeline.get(), frame_number);
    });
    rd->u->set_data(u);
    auto* const prs = rd->r.get();
    prs->set_mesh(msh->get_msh().get());
    prs->set_material(msh->get_mat().get());
    kernel->secondary_cmd->bind(prs);
}

gearoenix::render::graph::node::Unlit::Unlit(
    std::string name,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
    : Node(
        std::move(name),
        Type::Unlit,
        e,
        pipeline::Type::Unlit,
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
        f = std::make_unique<UnlitFrame>(e);
    }
}

gearoenix::render::graph::node::Unlit::~Unlit() noexcept = default;

void gearoenix::render::graph::node::Unlit::update() noexcept
{
    Node::update();
    frame_number = static_cast<std::size_t>(e->get_frame_number());
    frame = frames[frame_number].get();
    auto& kernels = frame->kernels;
    for (auto& kernel : kernels) {
        kernel->render_data_pool.refresh();
        kernel->secondary_cmd->begin();
    }
    meshes.clear();
}

void gearoenix::render::graph::node::Unlit::set_camera(const camera::Camera* const c) noexcept
{
    cam = c;
    render_target = c->get_target();
}

void gearoenix::render::graph::node::Unlit::add_mesh(std::pair<model::Model*, model::Mesh*> m) noexcept
{
    meshes.push_back(std::move(m));
}

void gearoenix::render::graph::node::Unlit::record(const unsigned int kernel_index) noexcept
{
    const unsigned int kernels_count = e->get_kernels()->get_threads_count();
    unsigned int task_number = 0;
    auto* const kernel = frame->kernels[kernel_index].get();
    model::Model* pre_mdl = nullptr;
    UnlitUniform u {};
    for (const auto& [mdl, msh] : meshes) {
        if (task_number == kernel_index) {
            if (pre_mdl != mdl) {
                u.mvp = math::Mat4x4<float>(math::Mat4x4<double>(cam->get_uniform().view_projection) * mdl->get_collider()->get_model_matrix());
                pre_mdl = mdl;
            }
            record(msh, u, kernel);
        }
        ++task_number;
        task_number %= kernels_count;
    }
}

void gearoenix::render::graph::node::Unlit::record_continuously(const unsigned int kernel_index) noexcept
{
    auto* const kernel = frame->kernels[kernel_index].get();
    model::Model* pre_mdl = nullptr;
    UnlitUniform u {};
    for (const auto& [mdl, msh] : meshes) {
        if (pre_mdl != mdl) {
            u.mvp = math::Mat4x4<float>(math::Mat4x4<double>(cam->get_uniform().view_projection) * mdl->get_collider()->get_model_matrix());
            pre_mdl = mdl;
        }
        record(msh, u, kernel);
    }
}

void gearoenix::render::graph::node::Unlit::submit() noexcept
{
    command::Buffer* cmd = frames_primary_cmd[frame_number].get();
    cmd->bind(render_target);
    for (const auto& k : frame->kernels) {
        cmd->record(k->secondary_cmd.get());
    }
    Node::submit();
}
