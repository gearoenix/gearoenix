#include "rnd-gr-nd-forward-pbr-directional-shadow.hpp"
#include "../../../core/asset/cr-asset-manager.hpp"
#include "../../../core/sync/cr-sync-kernel-workers.hpp"
#include "../../../system/sys-app.hpp"
#include "../../buffer/rnd-buf-manager.hpp"
#include "../../buffer/rnd-buf-uniform.hpp"
#include "../../camera/rnd-cmr-camera.hpp"
#include "../../command/rnd-cmd-buffer.hpp"
#include "../../command/rnd-cmd-manager.hpp"
#include "../../engine/rnd-eng-engine.hpp"
#include "../../light/rnd-lt-cascade-info.hpp"
#include "../../light/rnd-lt-directional.hpp"
#include "../../material/rnd-mat-material.hpp"
#include "../../mesh/rnd-msh-mesh.hpp"
#include "../../model/rnd-mdl-mesh.hpp"
#include "../../model/rnd-mdl-model.hpp"
#include "../../pipeline/rnd-pip-forward-pbr-directional-shadow-resource-set.hpp"
#include "../../pipeline/rnd-pip-forward-pbr-directional-shadow.hpp"
#include "../../pipeline/rnd-pip-manager.hpp"
#include "../../pipeline/rnd-pip-pipeline.hpp"
#include "../../pipeline/rnd-pip-resource-set.hpp"
#include "../../sync/rnd-sy-semaphore.hpp"
#include "../../texture/rnd-txt-manager.hpp"
#include "../../texture/rnd-txt-target.hpp"
#include "../../texture/rnd-txt-texture-2d.hpp"
#include "../../texture/rnd-txt-texture-cube.hpp"
#include <thread>

const unsigned int gearoenix::render::graph::node::ForwardPbrDirectionalShadow::DIFFUSE_ENVIRONMENT_INDEX = 0;
const unsigned int gearoenix::render::graph::node::ForwardPbrDirectionalShadow::SPECULAR_ENVIRONMENT_INDEX = 1;
const unsigned int gearoenix::render::graph::node::ForwardPbrDirectionalShadow::AMBIENT_OCCLUSION_INDEX = 2;
const unsigned int gearoenix::render::graph::node::ForwardPbrDirectionalShadow::SHADOW_MAP_INDEX = 3;
const unsigned int gearoenix::render::graph::node::ForwardPbrDirectionalShadow::BRDFLUT_INDEX = 4;

gearoenix::render::graph::node::ForwardPbrDirectionalShadowUniform::ForwardPbrDirectionalShadowUniform(const light::CascadeInfo* const cas) noexcept
{
    const auto& data = cas->get_cascades_data();
    const auto s = data.size();
    cascades_count = static_cast<core::Real>(s);
    for (std::size_t i = 0; i < s; ++i) {
        cascades_view_projections_bias[i] = data[i].view_projection_bias;
    }
}

gearoenix::render::graph::node::ForwardPbrDirectionalShadowRenderData::ForwardPbrDirectionalShadowRenderData(engine::Engine* const e, pipeline::Pipeline* const pip) noexcept
    : r(reinterpret_cast<pipeline::ForwardPbrDirectionalShadowResourceSet*>(pip->create_resource_set()))
    , u(e->get_buffer_manager()->create_uniform(sizeof(ForwardPbrDirectionalShadowUniform)))
{
    r->set_node_uniform_buffer(u.get());
}

gearoenix::render::graph::node::ForwardPbrDirectionalShadow::ForwardPbrDirectionalShadow(
    engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
    : Node(
        e,
        pipeline::Type::ForwardPbrDirectionalShadow,
        5,
        1,
        {
            "diffuse environment",
            "specular environment",
            "ambient occlusion",
            "shadow map",
        },
        {
            "color",
        },
        call)
    , frames(e->get_frames_count())
{

    for (auto& f : frames) {
        f = std::make_unique<ForwardPbrDirectionalShadowFrame>(e);
    }
    const std::shared_ptr<texture::Manager>& txtmgr = e->get_system_application()->get_asset_manager()->get_texture_manager();
    core::sync::EndCaller<texture::Cube> txtcubecall([call](std::shared_ptr<texture::Cube>) {});
    core::sync::EndCaller<texture::Texture2D> txt2dcall([call](std::shared_ptr<texture::Texture2D>) {});

    input_textures[DIFFUSE_ENVIRONMENT_INDEX] = txtmgr->get_cube(math::Vec3(0.0f, 0.0f, 0.0f), txtcubecall);
    input_textures[SPECULAR_ENVIRONMENT_INDEX] = txtmgr->get_cube(math::Vec3(0.0f, 0.0f, 0.0f), txtcubecall);
    input_textures[AMBIENT_OCCLUSION_INDEX] = txtmgr->get_2d(1.0f, txt2dcall);
    input_textures[SHADOW_MAP_INDEX] = txtmgr->get_2d(math::Vec2(1.0, 1.0), txt2dcall);
    input_textures[BRDFLUT_INDEX] = txtmgr->get_2d(math::Vec2(1.0, 1.0), txt2dcall);
}

void gearoenix::render::graph::node::ForwardPbrDirectionalShadow::set_diffuse_environment(const std::shared_ptr<texture::Cube>& t) noexcept
{
    set_input_texture(t, DIFFUSE_ENVIRONMENT_INDEX);
}

void gearoenix::render::graph::node::ForwardPbrDirectionalShadow::set_specular_environment(const std::shared_ptr<texture::Cube>& t) noexcept
{
    set_input_texture(t, SPECULAR_ENVIRONMENT_INDEX);
}

void gearoenix::render::graph::node::ForwardPbrDirectionalShadow::set_ambient_occlusion(const std::shared_ptr<texture::Texture2D>& t) noexcept
{
    set_input_texture(t, AMBIENT_OCCLUSION_INDEX);
}

void gearoenix::render::graph::node::ForwardPbrDirectionalShadow::set_shadow_mapper(const std::shared_ptr<texture::Texture2D>& t) noexcept
{
    set_input_texture(t, SHADOW_MAP_INDEX);
}

void gearoenix::render::graph::node::ForwardPbrDirectionalShadow::set_brdflut(const std::shared_ptr<texture::Texture2D>& t) noexcept
{
    set_input_texture(t, BRDFLUT_INDEX);
}

void gearoenix::render::graph::node::ForwardPbrDirectionalShadow::update() noexcept
{
    Node::update();
    const unsigned int frame_number = e->get_frame_number();
    frame = frames[frame_number].get();
    for (auto& kernel : frame->kernels) {
        kernel->render_data_pool.refresh();
        kernel->secondary_cmd->begin();
    }
}

void gearoenix::render::graph::node::ForwardPbrDirectionalShadow::record(
    const scene::Scene* const s,
    const camera::Camera* const c,
    const light::Directional* const l,
    const model::Model* const m,
    const light::CascadeInfo* const cas,
    const unsigned int kernel_index) noexcept
{
    auto& kernel = frame->kernels[kernel_index];
    const std::map<core::Id, std::shared_ptr<model::Mesh>>& meshes = m->get_meshes();
    for (const std::pair<const core::Id, std::shared_ptr<model::Mesh>>& id_mesh : meshes) {
        const std::shared_ptr<mesh::Mesh>& msh = id_mesh.second->get_mesh();
        const std::shared_ptr<material::Material>& mat = id_mesh.second->get_material();
        auto* const rd = kernel->render_data_pool.get_next([this] {
            return new ForwardPbrDirectionalShadowRenderData(e, render_pipeline.get());
        });
        ForwardPbrDirectionalShadowUniform u(cas);
        rd->u->update(&u);
        auto* const prs = rd->r.get();
        prs->set_scene(s);
        prs->set_camera(c);
        prs->set_light(l);
        prs->set_model(m);
        prs->set_mesh(msh.get());
        prs->set_material(mat.get());
        prs->set_diffuse_environment(reinterpret_cast<texture::Cube*>(input_textures[0].get()));
        prs->set_specular_environment(reinterpret_cast<texture::Cube*>(input_textures[1].get()));
        prs->set_ambient_occlusion(reinterpret_cast<texture::Texture2D*>(input_textures[2].get()));
        prs->set_shadow_mapper(reinterpret_cast<texture::Texture2D*>(input_textures[3].get()));
        prs->set_brdflut(reinterpret_cast<texture::Texture2D*>(input_textures[4].get()));
        kernel->secondary_cmd->bind(prs);
    }
}

void gearoenix::render::graph::node::ForwardPbrDirectionalShadow::submit() noexcept
{
    const unsigned int frame_number = e->get_frame_number();
    command::Buffer* cmd = frames_primary_cmd[frame_number].get();
    cmd->bind(render_target.get());
    for (const auto& k : frame->kernels) {
        cmd->record(k->secondary_cmd.get());
    }
    Node::submit();
}

gearoenix::render::graph::node::ForwardPbrDirectionalShadowFrame::ForwardPbrDirectionalShadowFrame(engine::Engine* e) noexcept
    : kernels(e->get_kernels()->get_threads_count())
{
    for (std::size_t i = 0; i < kernels.size(); ++i) {
        kernels[i] = std::make_unique<ForwardPbrDirectionalShadowKernel>(e, i);
    }
}

gearoenix::render::graph::node::ForwardPbrDirectionalShadowKernel::ForwardPbrDirectionalShadowKernel(engine::Engine* e, const unsigned int kernel_index) noexcept
    : secondary_cmd(e->get_command_manager()->create_secondary_command_buffer(kernel_index))
{
}
