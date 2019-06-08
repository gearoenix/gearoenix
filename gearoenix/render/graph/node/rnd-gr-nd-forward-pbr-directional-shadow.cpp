#include "rnd-gr-nd-forward-pbr-directional-shadow.hpp"
#include "../../../core/asset/cr-asset-manager.hpp"
#include "../../../system/sys-app.hpp"
#include "../../buffer/rnd-buf-manager.hpp"
#include "../../buffer/rnd-buf-uniform.hpp"
#include "../../camera/rnd-cmr-camera.hpp"
#include "../../command/rnd-cmd-buffer.hpp"
#include "../../command/rnd-cmd-manager.hpp"
#include "../../engine/rnd-eng-engine.hpp"
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

const unsigned int gearoenix::render::graph::node::ForwardPbrDirectionalShadow::diffuse_environment_index = 0;
const unsigned int gearoenix::render::graph::node::ForwardPbrDirectionalShadow::specular_environment_index = 1;
const unsigned int gearoenix::render::graph::node::ForwardPbrDirectionalShadow::ambient_occlusion_index = 2;
const unsigned int gearoenix::render::graph::node::ForwardPbrDirectionalShadow::shadow_map_index = 3;
const unsigned int gearoenix::render::graph::node::ForwardPbrDirectionalShadow::brdflut_index = 4;

gearoenix::render::graph::node::ForwardPbrDirectionalShadow::ForwardPbrDirectionalShadow(
    engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
    : Node(
        e,
        pipeline::Type::ForwardPbrDirectionalShadow,
        5,
        1,
        { "diffuse environment"
          "specular environment"
          "ambient occlusion"
          "shadow map" },
        { "color" },
        call)
{
    frames.resize(e->get_frames_count());
    for (unsigned int i = 0; i < e->get_frames_count(); ++i) {
        frames[i] = std::make_shared<ForwardPbrDirectionalShadowFrame>(e);
    }
    const std::shared_ptr<texture::Manager>& txtmgr = e->get_system_application()->get_asset_manager()->get_texture_manager();
    core::sync::EndCaller<texture::Cube> txtcubecall([call](std::shared_ptr<texture::Cube>) {});
    core::sync::EndCaller<texture::Texture2D> txt2dcall([call](std::shared_ptr<texture::Texture2D>) {});

    input_textures[diffuse_environment_index] = txtmgr->get_cube(math::Vec3(0.0f, 0.0f, 0.0f), txtcubecall);
    input_textures[specular_environment_index] = txtmgr->get_cube(math::Vec3(0.0f, 0.0f, 0.0f), txtcubecall);
    input_textures[ambient_occlusion_index] = txtmgr->get_2d(1.0f, txt2dcall);
    input_textures[shadow_map_index] = txtmgr->get_2d(math::Vec2(1.0, 1.0), txt2dcall);
    input_textures[brdflut_index] = txtmgr->get_2d(math::Vec2(1.0, 1.0), txt2dcall);
}

void gearoenix::render::graph::node::ForwardPbrDirectionalShadow::set_diffuse_environment(const std::shared_ptr<texture::Cube>& t) noexcept
{
    set_input_texture(t, diffuse_environment_index);
}

void gearoenix::render::graph::node::ForwardPbrDirectionalShadow::set_specular_environment(const std::shared_ptr<texture::Cube>& t) noexcept
{
    set_input_texture(t, specular_environment_index);
}

void gearoenix::render::graph::node::ForwardPbrDirectionalShadow::set_ambient_occlusion(const std::shared_ptr<texture::Texture2D>& t) noexcept
{
    set_input_texture(t, ambient_occlusion_index);
}

void gearoenix::render::graph::node::ForwardPbrDirectionalShadow::set_shadow_mapper(const std::shared_ptr<texture::Texture2D>& t) noexcept
{
    set_input_texture(t, shadow_map_index);
}

void gearoenix::render::graph::node::ForwardPbrDirectionalShadow::set_brdflut(const std::shared_ptr<texture::Texture2D>& t) noexcept
{
    set_input_texture(t, brdflut_index);
}

const std::shared_ptr<gearoenix::render::sync::Semaphore>& gearoenix::render::graph::node::ForwardPbrDirectionalShadow::get_semaphore(const unsigned int) noexcept
{
    GXUNEXPECTED;
}

void gearoenix::render::graph::node::ForwardPbrDirectionalShadow::update() noexcept
{
    const unsigned int frame_number = e->get_frame_number();
    const std::shared_ptr<ForwardPbrDirectionalShadowFrame>& frame = frames[frame_number];
    frame->primary_cmd->begin();
    for (const std::shared_ptr<ForwardPbrDirectionalShadowKernel>& kernel : frame->kernels) {
        kernel->render_data_pool.refresh();
        kernel->secondary_cmd->begin();
    }
}

void gearoenix::render::graph::node::ForwardPbrDirectionalShadow::record(
    const scene::Scene* const s,
    const camera::Camera* const c,
    const light::Directional* const l,
    const model::Model* const m,
    const unsigned int kernel_index) noexcept
{
    const unsigned int frame_number = e->get_frame_number();
    const std::shared_ptr<ForwardPbrDirectionalShadowFrame>& frame = frames[frame_number];
    const std::shared_ptr<ForwardPbrDirectionalShadowKernel>& kernel = frame->kernels[kernel_index];
    const std::map<core::Id, std::shared_ptr<model::Mesh>>& meshes = m->get_meshes();
    for (const std::pair<const core::Id, std::shared_ptr<model::Mesh>>& id_mesh : meshes) {
        const std::shared_ptr<mesh::Mesh>& msh = id_mesh.second->get_mesh();
        const std::shared_ptr<material::Material>& mat = id_mesh.second->get_material();
        pipeline::ForwardPbrDirectionalShadowResourceSet* const prs = kernel->render_data_pool.get_next([this] {
            return reinterpret_cast<pipeline::ForwardPbrDirectionalShadowResourceSet*>(render_pipeline->create_resource_set());
        });
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
        const std::shared_ptr<command::Buffer>& cmd = kernel->secondary_cmd;
        cmd->bind(prs);
    }
}

void gearoenix::render::graph::node::ForwardPbrDirectionalShadow::submit() noexcept
{
    const unsigned int frame_number = e->get_frame_number();
    const std::shared_ptr<ForwardPbrDirectionalShadowFrame>& frame = frames[frame_number];
    command::Buffer* cmd = frame->primary_cmd.get();
    cmd->bind(render_target);
    for (const std::shared_ptr<ForwardPbrDirectionalShadowKernel>& k : frame->kernels) {
        cmd->record(k->secondary_cmd.get());
    }
    std::vector<sync::Semaphore*> pss(providers.size());
    for (const std::shared_ptr<core::graph::Node>& p : providers) {
        if (p == nullptr)
            continue;
        const std::shared_ptr<Node>& rp = std::static_pointer_cast<Node>(p);
        if (rp == nullptr)
            continue;
        const std::shared_ptr<sync::Semaphore>& ps = rp->get_semaphore(frame_number);
        if (ps == nullptr)
            continue;
        pss.push_back(ps.get());
    }
    sync::Semaphore* nxt = frame->semaphore.get();
    e->submit(pss.size(), pss.data(), 1, &cmd, 1, &nxt);
}

gearoenix::render::graph::node::ForwardPbrDirectionalShadowFrame::ForwardPbrDirectionalShadowFrame(engine::Engine* e) noexcept
    : primary_cmd(e->get_command_manager()->create_primary_command_buffer())
    , semaphore(e->create_semaphore())
{
    const unsigned int kernels_count = std::thread::hardware_concurrency();
    kernels.resize(kernels_count);
    for (unsigned int i = 0; i < kernels_count; ++i) {
        kernels[i] = std::make_shared<ForwardPbrDirectionalShadowKernel>(e, i);
    }
}

gearoenix::render::graph::node::ForwardPbrDirectionalShadowKernel::ForwardPbrDirectionalShadowKernel(engine::Engine* e, const unsigned int kernel_index) noexcept
    : secondary_cmd(e->get_command_manager()->create_secondary_command_buffer(kernel_index))
{
}
