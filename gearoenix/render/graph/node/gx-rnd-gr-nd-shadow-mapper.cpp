#include "gx-rnd-gr-nd-shadow-mapper.hpp"
#include "../../../core/asset/gx-cr-asset-manager.hpp"
#include "../../../core/sync/gx-cr-sync-kernel-workers.hpp"
#include "../../buffer/gx-rnd-buf-framed-uniform.hpp"
#include "../../buffer/gx-rnd-buf-manager.hpp"
#include "../../command/gx-rnd-cmd-buffer.hpp"
#include "../../command/gx-rnd-cmd-manager.hpp"
#include "../../material/gx-rnd-mat-pbr.hpp"
#include "../../material/gx-rnd-mat-unlit.hpp"
#include "../../mesh/gx-rnd-msh-mesh.hpp"
#include "../../model/gx-rnd-mdl-mesh.hpp"
#include "../../model/gx-rnd-mdl-model.hpp"
#include "../../pipeline/gx-rnd-pip-shadow-mapper-resource-set.hpp"
#include "../../pipeline/gx-rnd-pip-shadow-mapper.hpp"
#include "../../sync/gx-rnd-sy-semaphore.hpp"
#include "../../texture/gx-rnd-txt-target.hpp"

gearoenix::render::graph::node::ShadowMapperRenderData::ShadowMapperRenderData(engine::Engine* const e, pipeline::Pipeline* const pip) noexcept
    : r(reinterpret_cast<pipeline::ShadowMapperResourceSet*>(pip->create_resource_set()))
    , u(e->get_buffer_manager()->create_uniform(sizeof(ShadowMapperUniform)))
{
    r->set_node_uniform_buffer(u.get());
}

gearoenix::render::graph::node::ShadowMapperRenderData::~ShadowMapperRenderData() noexcept
{
    r = nullptr;
    u = nullptr;
}

gearoenix::render::graph::node::ShadowMapperKernel::ShadowMapperKernel(
    engine::Engine* const e,
    const unsigned int kernel_index) noexcept
    : secondary_cmd(e->get_command_manager()->create_secondary_command_buffer(kernel_index))
{
}

gearoenix::render::graph::node::ShadowMapperKernel::~ShadowMapperKernel() noexcept
{
    secondary_cmd = nullptr;
}

gearoenix::render::graph::node::ShadowMapperFrame::ShadowMapperFrame(engine::Engine* const e) noexcept
{
    const unsigned int kernels_count = e->get_kernels()->get_threads_count();
    kernels.resize(kernels_count);
    for (unsigned int i = 0; i < kernels_count; ++i) {
        kernels[i] = std::make_unique<ShadowMapperKernel>(e, i);
    }
}

gearoenix::render::graph::node::ShadowMapperFrame::~ShadowMapperFrame() noexcept
{
    kernels.clear();
}

gearoenix::render::graph::node::ShadowMapper::ShadowMapper(
    std::string name,
    engine::Engine* e,
    const core::sync::EndCaller<gearoenix::core::sync::EndCallerIgnore>& call) noexcept
    : Node(
        std::move(name),
        Type::ShadowMapper,
        e,
        pipeline::Type::ShadowMapper,
        0,
        1,
        { "start" },
        { "depth" },
        call)
{
    frames.resize(e->get_frames_count());
    for (unsigned int i = 0; i < e->get_frames_count(); ++i) {
        frames[i] = std::make_unique<ShadowMapperFrame>(e);
    }
    const bool week_hwr = e->get_engine_type() == engine::Type::OpenGLES2;
    shadow_map_render_target = e->create_render_target(
        core::asset::Manager::create_id(),
        { texture::AttachmentInfo {
            .texture_info = texture::TextureInfo {
                .format = week_hwr ? texture::TextureFormat::D16 : texture::TextureFormat::D32,
                .sample_info = texture::SampleInfo {
                    .min_filter = texture::Filter::Nearest,
                    .mag_filter = texture::Filter::Nearest,
                    .wrap_s = texture::Wrap::ClampToEdge,
                    .wrap_t = texture::Wrap::ClampToEdge,
                    .wrap_r = texture::Wrap::ClampToEdge,
                },
                .texture_type = texture::Type::Texture2D,
                .has_mipmap = false,
            },
            .img_width = static_cast<unsigned int>(week_hwr ? 1024 : 2048),
            .img_height = static_cast<unsigned int>(week_hwr ? 1024 : 2048),
            .usage = texture::UsageFlag::Depth,
        } },
        call);
    render_target = shadow_map_render_target.get();
    output_textures[0] = shadow_map_render_target->get_texture(0);
}

gearoenix::render::graph::node::ShadowMapper::~ShadowMapper() noexcept
{
    frames.clear();
    frame = nullptr;
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

void gearoenix::render::graph::node::ShadowMapper::record_shadow(const math::Mat4x4<double>& mvp, const model::Model* const m, const std::size_t kernel_index) noexcept
{
    const auto& kernel = frame->kernels[kernel_index];
    const std::map<core::Id, std::shared_ptr<model::Mesh>>& meshes = m->get_meshes();
    for (const std::pair<const core::Id, std::shared_ptr<model::Mesh>>& id_mesh : meshes) {
        const auto* const mat = id_mesh.second->get_mat().get();
        const auto* const mat_buff = mat->get_uniform_buffers()->get_buffer();
        ShadowMapperUniform u;
        u.mvp = math::Mat4x4<float>(mvp);
        switch (mat->get_material_type()) {
        case material::Type::Unlit: {
            const auto* const ptr = mat_buff->get_ptr<material::Unlit::Uniform>();
            u.alpha = ptr->alpha;
            u.alpha_cutoff = ptr->alpha_cutoff;
            break;
        }
        case material::Type::Pbr: {
            const auto* const ptr = mat_buff->get_ptr<material::Pbr::Uniform>();
            u.alpha = ptr->alpha;
            u.alpha_cutoff = ptr->alpha_cutoff;
            break;
        }
        default:
            GX_UNEXPECTED
        }
        const auto* const msh = id_mesh.second->get_msh().get();
        auto* const rd = kernel->render_data_pool.get_next([this] {
            return new ShadowMapperRenderData(e, render_pipeline.get());
        });
        const auto& ub = rd->u;
        ub->set_data(u);
        const auto& prs = rd->r;
        prs->set_mesh(msh);
        prs->set_material(mat);
        const auto& cmd = kernel->secondary_cmd;
        cmd->bind(prs.get());
    }
}

void gearoenix::render::graph::node::ShadowMapper::submit() noexcept
{
    const unsigned int frame_number = e->get_frame_number();
    command::Buffer* cmd = frames_primary_cmd[frame_number].get();
    cmd->bind(render_target);
    for (const auto& k : frame->kernels) {
        cmd->record(k->secondary_cmd.get());
    }
    Node::submit();
}
