#include "gx-rnd-gr-nd-forward-pbr.hpp"
#include "../../../core/asset/gx-cr-asset-manager.hpp"
#include "../../../core/sync/gx-cr-sync-kernel-workers.hpp"
#include "../../../system/gx-sys-application.hpp"
#include "../../buffer/gx-rnd-buf-manager.hpp"
#include "../../buffer/gx-rnd-buf-uniform.hpp"
#include "../../camera/gx-rnd-cmr-camera.hpp"
#include "../../command/gx-rnd-cmd-buffer.hpp"
#include "../../command/gx-rnd-cmd-manager.hpp"
#include "../../engine/gx-rnd-eng-engine.hpp"
#include "../../light/gx-rnd-lt-cascade-info.hpp"
#include "../../light/gx-rnd-lt-directional.hpp"
#include "../../light/gx-rnd-lt-point.hpp"
#include "../../material/gx-rnd-mat-material.hpp"
#include "../../mesh/gx-rnd-msh-mesh.hpp"
#include "../../model/gx-rnd-mdl-mesh.hpp"
#include "../../model/gx-rnd-mdl-model.hpp"
#include "../../pipeline/gx-rnd-pip-forward-pbr-resource-set.hpp"
#include "../../pipeline/gx-rnd-pip-manager.hpp"
#include "../../pipeline/gx-rnd-pip-pipeline.hpp"
#include "../../scene/gx-rnd-scn-scene.hpp"
#include "../../sync/gx-rnd-sy-semaphore.hpp"
#include "../../texture/gx-rnd-txt-manager.hpp"
#include "../../texture/gx-rnd-txt-target.hpp"
#include "../../texture/gx-rnd-txt-texture-2d.hpp"
#include "../../texture/gx-rnd-txt-texture-cube.hpp"
#include "gx-rnd-gr-nd-shadow-mapper.hpp"

const unsigned int gearoenix::render::graph::node::ForwardPbr::DIFFUSE_ENVIRONMENT_INDEX = 0;
const unsigned int gearoenix::render::graph::node::ForwardPbr::SPECULAR_ENVIRONMENT_INDEX = 1;
const unsigned int gearoenix::render::graph::node::ForwardPbr::AMBIENT_OCCLUSION_INDEX = 2;
const unsigned int gearoenix::render::graph::node::ForwardPbr::BRDFLUT_INDEX = 3;
const unsigned int gearoenix::render::graph::node::ForwardPbr::SHADOW_MAP_000_INDEX = 4;

gearoenix::render::graph::node::ForwardPbrRenderData::ForwardPbrRenderData(
    engine::Engine* const e,
    pipeline::Pipeline* const pip,
    const std::size_t frame_number) noexcept
    : r(dynamic_cast<pipeline::ForwardPbrResourceSet*>(pip->create_resource_set()))
    , u(e->get_buffer_manager()->create_uniform(sizeof(ForwardPbrUniform), frame_number))
{
    r->set_node_uniform_buffer(u.get());
}

gearoenix::render::graph::node::ForwardPbrRenderData::~ForwardPbrRenderData() noexcept
{
    r = nullptr;
    u = nullptr;
}

void gearoenix::render::graph::node::ForwardPbr::record(
    const model::Model* const mdl,
    const model::Mesh* const msh,
    const ForwardPbrUniform& u,
    ForwardPbrKernel* const kernel) noexcept
{
    auto* const rd = kernel->render_data_pool.get_next([this] {
        return new ForwardPbrRenderData(e, render_pipeline.get(), frame_number);
    });
    rd->u->set_data(u);
    auto* const prs = rd->r.get();
    prs->set_scene(scn);
    prs->set_camera(cam);
    prs->set_model(mdl);
    prs->set_mesh(msh->get_msh().get());
    prs->set_material(msh->get_mat().get());
    prs->set_ambient_occlusion(reinterpret_cast<texture::Texture2D*>(input_textures[AMBIENT_OCCLUSION_INDEX]));
    prs->set_brdflut(reinterpret_cast<texture::Texture2D*>(input_textures[BRDFLUT_INDEX]));
    for (int di = 0, si = SHADOW_MAP_000_INDEX; di < GX_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER; ++di) {
        for (int ci = 0; ci < GX_MAX_SHADOW_CASCADES; ++ci, ++si) {
            prs->set_directional_lights_shadow_map(dynamic_cast<texture::Texture2D*>(input_textures[si]), di, ci);
        }
    }
    kernel->secondary_cmd->bind(prs);
}

gearoenix::render::graph::node::ForwardPbr::ForwardPbr(
    std::string name,
    engine::Engine* e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
    : Node(
        std::move(name),
        Type::ForwardPbr,
        e,
        pipeline::Type::ForwardPbr,
        4 + GX_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER * GX_MAX_SHADOW_CASCADES,
        1,
        {
            "irradiance environment",
            "radiance environment",
            "ambient occlusion",
        },
        {
            "color",
        },
        call)
    , frames(e->get_frames_count())
{
    set_providers_count(input_textures.size());
    for (auto& f : frames) {
        f = std::make_unique<ForwardPbrFrame>(e);
    }
    auto* const txt_mgr = e->get_system_application()->get_asset_manager()->get_texture_manager();
    core::sync::EndCaller<texture::TextureCube> txt_cube_call([call](const std::shared_ptr<texture::TextureCube>&) {});
    core::sync::EndCaller<texture::Texture2D> txt_2d_call([call](const std::shared_ptr<texture::Texture2D>&) {});

    input_textures[DIFFUSE_ENVIRONMENT_INDEX] = txt_mgr->get_cube_zero_3c(txt_cube_call).get();
    input_textures[SPECULAR_ENVIRONMENT_INDEX] = txt_mgr->get_cube_zero_3c(txt_cube_call).get();
    input_textures[AMBIENT_OCCLUSION_INDEX] = txt_mgr->get_2d_one_1c(txt_2d_call).get();
    input_textures[BRDFLUT_INDEX] = txt_mgr->get_brdflut(txt_2d_call).get();
}

gearoenix::render::graph::node::ForwardPbr::~ForwardPbr() noexcept
{
    frames.clear();
}

void gearoenix::render::graph::node::ForwardPbr::set_diffuse_environment(texture::TextureCube* const t) noexcept
{
    set_input_texture(t, DIFFUSE_ENVIRONMENT_INDEX);
}

void gearoenix::render::graph::node::ForwardPbr::set_specular_environment(texture::TextureCube* const t) noexcept
{
    set_input_texture(t, SPECULAR_ENVIRONMENT_INDEX);
}

void gearoenix::render::graph::node::ForwardPbr::set_ambient_occlusion(texture::Texture2D* const t) noexcept
{
    set_input_texture(t, AMBIENT_OCCLUSION_INDEX);
}

void gearoenix::render::graph::node::ForwardPbr::set_brdflut(texture::Texture2D* const t) noexcept
{
    set_input_texture(t, BRDFLUT_INDEX);
}

void gearoenix::render::graph::node::ForwardPbr::update() noexcept
{
    Node::update();
    frame_number = static_cast<std::size_t>(e->get_frame_number());
    frame = frames[frame_number].get();
    for (auto& kernel : frame->kernels) {
        kernel->render_data_pool.refresh();
        kernel->secondary_cmd->begin();
    }
    meshes.clear();
    cascades.clear();

    for (unsigned int di = 0, si = SHADOW_MAP_000_INDEX; di < GX_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER; ++di) {
        for (int ci = 0; ci < GX_MAX_SHADOW_CASCADES; ++ci, ++si) {
            clear_provider(si);
            input_textures[si] = nullptr;
        }
    }
    shadow_map_index = SHADOW_MAP_000_INDEX;
    directional_light_shadow_caster_index = 0;

    uniform.point_lights_count = 0.0f;
    uniform.shadow_caster_directional_lights_count = 0.0f;
}

void gearoenix::render::graph::node::ForwardPbr::set_scene(const scene::Scene* const s) noexcept
{
    scn = s;
    // Initializing point lights
    unsigned int lights_count = 0;
    const auto& lights = scn->get_lights();
    for (const auto& id_light : lights) {
        const auto& l = id_light.second;
        if (light::Type::Point == l->get_light_type()) {
            auto pl = reinterpret_cast<light::Point*>(l.get());
            if (GX_COUNT_OF(uniform.point_lights_color_min_radius) <= lights_count) {
                GXLOGD("Unexpected number of influencing point lights in scene: " << scn->get_id())
                break;
            }
            uniform.point_lights_color_min_radius[lights_count] = math::Vec4<float>(
                math::Vec3<float>(l->get_color()), static_cast<float>(pl->get_min_radius()));
            uniform.point_lights_position_max_radius[lights_count] = math::Vec4<float>(pl->get_position_max_radius());
            ++lights_count;
        }
    }
    uniform.point_lights_count = static_cast<float>(lights_count);
}

void gearoenix::render::graph::node::ForwardPbr::set_camera(const camera::Camera* const c) noexcept
{
    cam = c;
    render_target = c->get_target();
}

void gearoenix::render::graph::node::ForwardPbr::add_cascade(const light::CascadeInfo* const c) noexcept
{
    cascades.push_back(c);
    const auto& cds = c->get_cascades_data();
    int cascaded_index = 0;
    for (const light::CascadeInfo::PerCascade& pc : cds) {
        graph::node::ShadowMapper* const shm = pc.shadow_mapper.get();
        core::graph::Node::connect(shm, 0, this, shadow_map_index);
        input_textures[shadow_map_index] = shm->get_output_texture(0).get();
        ++cascaded_index;
        ++shadow_map_index;
    }
    // Initializing directional lights
    const auto* const dir = c->get_source();
    const auto s = cds.size();
    uniform.shadow_caster_directional_lights_color_cascades_count[directional_light_shadow_caster_index] = math::Vec4(math::Vec3<float>(dir->get_color()), static_cast<float>(s + 0.1));
    uniform.shadow_caster_directional_lights_direction[directional_light_shadow_caster_index] = math::Vec4(math::Vec3<float>(dir->get_direction()), 0.0f);
    for (std::size_t i = static_cast<std::size_t>(directional_light_shadow_caster_index) * GX_MAX_SHADOW_CASCADES, j = 0; j < s; ++i, ++j) {
#ifdef GX_USE_OPENGL
#ifdef GX_USE_INSTEAD_OF_OPENGL
        if (GX_RUNTIME_USE_OPENGL) {
#endif
            uniform.shadow_caster_directional_lights_cascades_view_projections_bias[i] = math::Mat4x4<float>(cds[j].view_projection_bias_gl);
#ifdef GX_USE_INSTEAD_OF_OPENGL
            continue;
        }
#endif
#endif
    }
    ++directional_light_shadow_caster_index;
    uniform.shadow_caster_directional_lights_count = static_cast<float>(directional_light_shadow_caster_index);
}

void gearoenix::render::graph::node::ForwardPbr::record(const unsigned int kernel_index) noexcept
{
    const unsigned int kernels_count = e->get_kernels()->get_threads_count();
    auto* const kernel = frame->kernels[kernel_index].get();
    GX_START_MULTITHREADED_TASKS
    for (const auto& [mdl, msh] : meshes) {
        GX_DO_MULTITHREADED_TASK(record(mdl, msh, uniform, kernel))
    }
}

void gearoenix::render::graph::node::ForwardPbr::record_continuously(unsigned int kernel_index) noexcept
{
    auto* const kernel = frame->kernels[kernel_index].get();
    for (const auto& [mdl, msh] : meshes) {
        record(mdl, msh, uniform, kernel);
    }
}

void gearoenix::render::graph::node::ForwardPbr::submit() noexcept
{
    command::Buffer* cmd = frames_primary_cmd[frame_number].get();
    cmd->bind(render_target);
    for (const auto& k : frame->kernels) {
        cmd->record(k->secondary_cmd.get());
    }
    Node::submit();
}

void gearoenix::render::graph::node::ForwardPbr::add_mesh(std::pair<const model::Model*, const model::Mesh*> m) noexcept
{
    meshes.push_back(std::move(m));
}

gearoenix::render::graph::node::ForwardPbrFrame::ForwardPbrFrame(engine::Engine* e) noexcept
    : kernels(e->get_kernels()->get_threads_count())
{
    for (std::size_t i = 0; i < kernels.size(); ++i) {
        kernels[i] = std::make_unique<ForwardPbrKernel>(e, static_cast<unsigned int>(i));
    }
}

gearoenix::render::graph::node::ForwardPbrFrame::~ForwardPbrFrame() noexcept
{
    kernels.clear();
}

gearoenix::render::graph::node::ForwardPbrKernel::ForwardPbrKernel(engine::Engine* e, const unsigned int kernel_index) noexcept
    : secondary_cmd(e->get_command_manager()->create_secondary_command_buffer(kernel_index))
{
}

gearoenix::render::graph::node::ForwardPbrKernel::~ForwardPbrKernel() noexcept
{
    secondary_cmd = nullptr;
}
