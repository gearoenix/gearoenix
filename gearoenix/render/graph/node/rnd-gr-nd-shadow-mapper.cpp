#include "rnd-gr-nd-shadow-mapper.hpp"
#include "../../../core/asset/cr-asset-manager.hpp"
#include "../../../core/sync/cr-sync-kernel-workers.hpp"
#include "../../buffer/rnd-buf-manager.hpp"
#include "../../buffer/rnd-buf-uniform.hpp"
#include "../../command/rnd-cmd-buffer.hpp"
#include "../../command/rnd-cmd-manager.hpp"
#include "../../engine/rnd-eng-engine.hpp"
#include "../../material/rnd-mat-material.hpp"
#include "../../model/rnd-mdl-mesh.hpp"
#include "../../model/rnd-mdl-model.hpp"
#include "../../pipeline/rnd-pip-shadow-mapper-resource-set.hpp"
#include "../../pipeline/rnd-pip-shadow-mapper.hpp"
#include "../../sync/rnd-sy-semaphore.hpp"
#include "../../texture/rnd-txt-target.hpp"

gearoenix::render::graph::node::ShadowMapperRenderData::ShadowMapperRenderData(engine::Engine* const e, pipeline::Pipeline* const pip) noexcept
{
    r = reinterpret_cast<pipeline::ShadowMapperResourceSet*>(pip->create_resource_set());
    u = e->get_buffer_manager()->create_uniform(sizeof(ShadowMapperUniform));
    r->set_node_uniform_buffer(u);
}

gearoenix::render::graph::node::ShadowMapperRenderData::~ShadowMapperRenderData() noexcept
{
    delete r;
    delete u;
}

gearoenix::render::graph::node::ShadowMapperKernel::ShadowMapperKernel(
    engine::Engine* const e,
    const unsigned int kernel_index) noexcept
    : secondary_cmd(e->get_command_manager()->create_secondary_command_buffer(kernel_index))
{
}

gearoenix::render::graph::node::ShadowMapperFrame::ShadowMapperFrame(engine::Engine* const e) noexcept
{
    const unsigned int kernels_count = e->get_kernels()->get_threads_count();
    kernels.resize(kernels_count);
    for (unsigned int i = 0; i < kernels_count; ++i) {
        kernels[i] = std::make_shared<ShadowMapperKernel>(e, i);
    }
}

gearoenix::render::graph::node::ShadowMapper::ShadowMapper(
    engine::Engine* e,
    const core::sync::EndCaller<gearoenix::core::sync::EndCallerIgnore>& call) noexcept
    : Node(e, pipeline::Type::ShadowMapper, 0, 1, { "start" }, { "depth" }, call)
{
    frames.resize(e->get_frames_count());
    for (unsigned int i = 0; i < e->get_frames_count(); ++i) {
        frames[i] = std::make_shared<ShadowMapperFrame>(e);
    }
    const bool week_hwr = e->get_engine_type_id() == engine::Type::OPENGL_ES2;
    texture::SampleInfo s {};
    std::vector<texture::Info> txt_infoes = { texture::Info() };
    txt_infoes[0].f = week_hwr ? texture::TextureFormat::D_16 : texture::TextureFormat::D_32;
    render_target = std::shared_ptr<texture::Target>(e->create_render_target(
        core::asset::Manager::create_id(),
        txt_infoes,
        week_hwr ? 512 : 1024,
        week_hwr ? 512 : 1024,
        call));
    output_textures[0] = render_target.get();
}

void gearoenix::render::graph::node::ShadowMapper::update() noexcept
{
    Node::update();
    const unsigned int frame_number = e->get_frame_number();
    frame = frames[frame_number].get();
    for (const auto& kernel : frame->kernels) {
        kernel->render_data_pool.refresh();
        kernel->secondary_cmd->begin();
    }
}

void gearoenix::render::graph::node::ShadowMapper::record(const math::Mat4x4& mvp, const model::Model* const m, const unsigned int kernel_index) noexcept
{
    const auto& kernel = frame->kernels[kernel_index];
    const std::map<core::Id, std::shared_ptr<model::Mesh>>& meshes = m->get_meshes();
    for (const std::pair<const core::Id, std::shared_ptr<model::Mesh>>& id_mesh : meshes) {
        const auto& mat = id_mesh.second->get_material();
        /// TODO: check for shadow casting
        /// if(!mat->is_shadow_caster()) continue;
        const auto& msh = id_mesh.second->get_mesh();
        auto* const rd = kernel->render_data_pool.get_next([this] {
            return new ShadowMapperRenderData(e, render_pipeline.get());
        });
        auto* ub = rd->u;
        ub->update(&mvp);
        auto* prs = rd->r;
        prs->set_mesh(msh.get());
        prs->set_material(mat.get());
        const std::shared_ptr<command::Buffer>& cmd = kernel->secondary_cmd;
        cmd->bind(prs);
    }
}

void gearoenix::render::graph::node::ShadowMapper::submit() noexcept
{
    const unsigned int frame_number = e->get_frame_number();
    command::Buffer* cmd = frames_primary_cmd[frame_number];
    cmd->bind(render_target.get());
    for (const auto& k : frame->kernels) {
        cmd->record(k->secondary_cmd.get());
    }
    Node::submit();
}
