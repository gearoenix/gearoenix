#include "rnd-gr-nd-sky-equirectangular.hpp"
#include "../../../core/asset/cr-asset-manager.hpp"
#include "../../../core/sync/cr-sync-kernel-workers.hpp"
#include "../../../system/sys-app.hpp"
#include "../../buffer/rnd-buf-manager.hpp"
#include "../../buffer/rnd-buf-uniform.hpp"
#include "../../camera/rnd-cmr-camera.hpp"
#include "../../command/rnd-cmd-buffer.hpp"
#include "../../command/rnd-cmd-manager.hpp"
#include "../../engine/rnd-eng-engine.hpp"
#include "../../light/rnd-lt-point.hpp"
#include "../../material/rnd-mat-material.hpp"
#include "../../mesh/rnd-msh-mesh.hpp"
#include "../../model/rnd-mdl-mesh.hpp"
#include "../../model/rnd-mdl-model.hpp"
#include "../../pipeline/rnd-pip-manager.hpp"
#include "../../pipeline/rnd-pip-pipeline.hpp"
#include "../../pipeline/rnd-pip-sky-equirectangular-resource-set.hpp"
#include "../../scene/rnd-scn-scene.hpp"
#include "../../sync/rnd-sy-semaphore.hpp"
#include "../../texture/rnd-txt-manager.hpp"
#include "../../texture/rnd-txt-target.hpp"
#include "../../texture/rnd-txt-texture-cube.hpp"

gearoenix::render::graph::node::SkyEquirectangularRenderData::SkyEquirectangularRenderData(engine::Engine* e, pipeline::Pipeline* pip) noexcept
    : r(dynamic_cast<pipeline::SkyEquirectangularResourceSet*>(pip->create_resource_set()))
    , u(e->get_buffer_manager()->create_uniform(sizeof(SkyEquirectangularUniform)))
{
    r->set_node_uniform_buffer(u.get());
}

gearoenix::render::graph::node::SkyEquirectangularRenderData::~SkyEquirectangularRenderData() noexcept = default;

gearoenix::render::graph::node::SkyEquirectangularKernel::SkyEquirectangularKernel(engine::Engine* e, const unsigned int kernel_index) noexcept
    : secondary_cmd(e->get_command_manager()->create_secondary_command_buffer(kernel_index))
{
}

gearoenix::render::graph::node::SkyEquirectangularKernel::~SkyEquirectangularKernel() noexcept = default;

gearoenix::render::graph::node::SkyEquirectangularFrame::SkyEquirectangularFrame(gearoenix::render::engine::Engine* e) noexcept
    : kernels(e->get_kernels()->get_threads_count())
{
    for (std::size_t i = 0; i < kernels.size(); ++i) {
        kernels[i] = std::make_unique<SkyEquirectangularKernel>(e, static_cast<unsigned int>(i));
    }
}

gearoenix::render::graph::node::SkyEquirectangularFrame::~SkyEquirectangularFrame() noexcept = default;

void gearoenix::render::graph::node::SkyEquirectangular::record(
    const model::Mesh* const msh,
    const SkyEquirectangularUniform& u,
    SkyEquirectangularKernel* const kernel) noexcept
{
    auto* const rd = kernel->render_data_pool.get_next([this] {
        return new SkyEquirectangularRenderData(e, render_pipeline.get());
    });
    rd->u->set_data(u);
    auto* const prs = rd->r.get();
    prs->set_mesh(msh->get_msh().get());
    prs->set_material(msh->get_mat().get());
    kernel->secondary_cmd->bind(prs);
}

gearoenix::render::graph::node::SkyEquirectangular::SkyEquirectangular(
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
    : Node(
        Type::SkyEquirectangular,
        e,
        pipeline::Type::SkyEquirectangular,
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
        f = std::make_unique<SkyEquirectangularFrame>(e);
    }
}

gearoenix::render::graph::node::SkyEquirectangular::~SkyEquirectangular() noexcept = default;

void gearoenix::render::graph::node::SkyEquirectangular::update() noexcept
{
    Node::update();
    const unsigned int frame_number = e->get_frame_number();
    frame = frames[frame_number].get();
    auto& kernels = frame->kernels;
    for (auto& kernel : kernels) {
        kernel->render_data_pool.refresh();
        kernel->secondary_cmd->begin();
    }
    models.clear();
}

void gearoenix::render::graph::node::SkyEquirectangular::set_camera(const camera::Camera* const c) noexcept
{
    cam = c;
    const auto& cam_uni = cam->get_uniform();
    render_target->set_clipping(cam_uni.clip_width, cam_uni.clip_height);
}

void gearoenix::render::graph::node::SkyEquirectangular::add_models(const std::map<const model::Model*, std::vector<const model::Mesh*>>* ms) noexcept
{
    models.push_back(ms);
}

void gearoenix::render::graph::node::SkyEquirectangular::record(const unsigned int kernel_index) noexcept
{
    const unsigned int kernels_count = e->get_kernels()->get_threads_count();
    unsigned int task_number = 0;
    auto* const kernel = frame->kernels[kernel_index].get();
    for (auto* const models_meshes : models) {
        for (const auto& model_meshes : *models_meshes) {
            if (task_number == kernel_index) {
                const SkyEquirectangularUniform u {
                    .mvp = cam->get_uniform().view_projection * model_meshes.first->get_collider()->get_model_matrix()
                };
                for (const auto* const msh : model_meshes.second) {
                    record(msh, u, kernel);
                }
            }
            ++task_number;
            task_number %= kernels_count;
        }
    }
}

void gearoenix::render::graph::node::SkyEquirectangular::record_continuously(const unsigned int kernel_index) noexcept
{
    auto* const kernel = frame->kernels[kernel_index].get();
    for (auto* const models_meshes : models) {
        for (const auto& model_meshes : *models_meshes) {
            const SkyEquirectangularUniform u {
                .mvp = cam->get_uniform().view_projection * model_meshes.first->get_collider()->get_model_matrix()
            };
            for (const auto* const msh : model_meshes.second) {
                record(msh, u, kernel);
            }
        }
    }
}

void gearoenix::render::graph::node::SkyEquirectangular::submit() noexcept
{
    const unsigned int frame_number = e->get_frame_number();
    command::Buffer* cmd = frames_primary_cmd[frame_number].get();
    cmd->bind(render_target.get());
    for (const auto& k : frame->kernels) {
        cmd->record(k->secondary_cmd.get());
    }
    Node::submit();
}
