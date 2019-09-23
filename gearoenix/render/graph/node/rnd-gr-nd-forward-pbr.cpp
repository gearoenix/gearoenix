#include "rnd-gr-nd-forward-pbr.hpp"
#include "../../../core/asset/cr-asset-manager.hpp"
#include "../../../core/cr-static.hpp"
#include "../../../core/sync/cr-sync-kernel-workers.hpp"
#include "../../../system/sys-app.hpp"
#include "../../buffer/rnd-buf-manager.hpp"
#include "../../buffer/rnd-buf-uniform.hpp"
#include "../../camera/rnd-cmr-camera.hpp"
#include "../../camera/rnd-cmr-uniform.hpp"
#include "../../command/rnd-cmd-buffer.hpp"
#include "../../command/rnd-cmd-manager.hpp"
#include "../../engine/rnd-eng-engine.hpp"
#include "../../light/rnd-lt-cascade-info.hpp"
#include "../../light/rnd-lt-directional.hpp"
#include "../../light/rnd-lt-point.hpp"
#include "../../material/rnd-mat-material.hpp"
#include "../../mesh/rnd-msh-mesh.hpp"
#include "../../model/rnd-mdl-mesh.hpp"
#include "../../model/rnd-mdl-model.hpp"
#include "../../pipeline/rnd-pip-forward-pbr-resource-set.hpp"
#include "../../pipeline/rnd-pip-forward-pbr.hpp"
#include "../../pipeline/rnd-pip-manager.hpp"
#include "../../pipeline/rnd-pip-pipeline.hpp"
#include "../../pipeline/rnd-pip-resource-set.hpp"
#include "../../scene/rnd-scn-scene.hpp"
#include "../../sync/rnd-sy-semaphore.hpp"
#include "../../texture/rnd-txt-manager.hpp"
#include "../../texture/rnd-txt-target.hpp"
#include "../../texture/rnd-txt-texture-2d.hpp"
#include "../../texture/rnd-txt-texture-cube.hpp"
#include "rnd-gr-nd-shadow-mapper.hpp"
#include <thread>

const unsigned int gearoenix::render::graph::node::ForwardPbr::DIFFUSE_ENVIRONMENT_INDEX = 0;
const unsigned int gearoenix::render::graph::node::ForwardPbr::SPECULAR_ENVIRONMENT_INDEX = 1;
const unsigned int gearoenix::render::graph::node::ForwardPbr::AMBIENT_OCCLUSION_INDEX = 2;
const unsigned int gearoenix::render::graph::node::ForwardPbr::BRDFLUT_INDEX = 3;
const unsigned int gearoenix::render::graph::node::ForwardPbr::SHADOW_MAP_000_INDEX = 4;

gearoenix::render::graph::node::ForwardPbrUniform::ForwardPbrUniform(
    const std::vector<std::pair<light::Directional*, light::CascadeInfo*>>* const directional_lights,
    const scene::Scene* const scn,
    const model::Model* const mdl) noexcept
{
    // Initializing point lights
    unsigned int lights_count = 0;
    const auto& lights = scn->get_lights();
    for (const auto& id_light : lights) {
        const auto& l = id_light.second;
        if (light::Type::POINT == l->get_type()) {
            auto pl = reinterpret_cast<light::Point*>(l.get());
            if (l->is_in_light(mdl)) {
                if (GX_COUNT_OF(point_lights_color_min_radius) <= lights_count) {
                    GXLOGD("Unexpected number of influencing point lights over model: " << mdl->get_asset_id())
                    break;
                }
                point_lights_color_min_radius[lights_count] = math::Vec4(l->get_color(), pl->get_min_radius());
                point_lights_position_max_radius[lights_count] = pl->get_position_max_radius();
                ++lights_count;
            }
        }
    }
    point_lights_count = static_cast<core::Real>(lights_count);
    // Initializing directional lights
    lights_count = 0;
    for (const auto& dir_cas : *directional_lights) {
        auto dir = dir_cas.first;
        auto cas = dir_cas.second;
        const auto& data = cas->get_cascades_data();
        const auto s = data.size();
        shadow_caster_directional_lights_color_cascades_count[lights_count] = math::Vec4(dir->get_color(), static_cast<core::Real>(s) + 0.1f);
        shadow_caster_directional_lights_direction[lights_count] = math::Vec4(dir->get_direction(), 0.0f);
        for (std::size_t i = static_cast<std::size_t>(lights_count) * GX_MAX_SHADOW_CASCADES, j = 0; j < s; ++i, ++j) {
#ifdef GX_USE_OPENGL
#ifdef GX_USE_INSTEAD_OF_OPENGL
            if (GX_RUNTIME_USE_OPENGL) {
#endif
                shadow_caster_directional_lights_cascades_view_projections_bias[i] = data[i].view_projection_bias_gl;
#ifdef GX_USE_INSTEAD_OF_OPENGL
                continue;
            }
#endif
#endif
        }
        ++lights_count;
    }
    shadow_caster_directional_lights_count = static_cast<core::Real>(lights_count);
}

gearoenix::render::graph::node::ForwardPbrRenderData::ForwardPbrRenderData(engine::Engine* const e, pipeline::Pipeline* const pip) noexcept
    : r(reinterpret_cast<pipeline::ForwardPbrResourceSet*>(pip->create_resource_set()))
    , u(e->get_buffer_manager()->create_uniform(sizeof(ForwardPbrUniform)))
{
    r->set_node_uniform_buffer(u.get());
}

gearoenix::render::graph::node::ForwardPbrRenderData::~ForwardPbrRenderData() noexcept
{
    r = nullptr;
    u = nullptr;
}

void gearoenix::render::graph::node::ForwardPbr::record(
    const model::Model* const m,
    const unsigned int kernel_index) noexcept
{
    const ForwardPbrUniform u(directional_lights, scn, m);
    const auto& cam_uni = cam->get_uniform();
    render_target->set_clipping(cam_uni.clip_width, cam_uni.clip_height);
    auto& kernel = frame->kernels[kernel_index];
    const std::map<core::Id, std::shared_ptr<model::Mesh>>& meshes = m->get_meshes();
    for (const std::pair<const core::Id, std::shared_ptr<model::Mesh>>& id_mesh : meshes) {
        const std::shared_ptr<mesh::Mesh>& msh = id_mesh.second->get_mesh();
        const std::shared_ptr<material::Material>& mat = id_mesh.second->get_material();
        auto* const rd = kernel->render_data_pool.get_next([this] {
            return new ForwardPbrRenderData(e, render_pipeline.get());
        });
        rd->u->set_data(u);
        auto* const prs = rd->r.get();
        prs->set_scene(scn);
        prs->set_camera(cam);
        prs->set_model(m);
        prs->set_mesh(msh.get());
        prs->set_material(mat.get());
        prs->set_diffuse_environment(reinterpret_cast<texture::Cube*>(input_textures[DIFFUSE_ENVIRONMENT_INDEX]));
        prs->set_specular_environment(reinterpret_cast<texture::Cube*>(input_textures[SPECULAR_ENVIRONMENT_INDEX]));
        prs->set_ambient_occlusion(reinterpret_cast<texture::Texture2D*>(input_textures[AMBIENT_OCCLUSION_INDEX]));
        prs->set_brdflut(reinterpret_cast<texture::Texture2D*>(input_textures[BRDFLUT_INDEX]));
        for (int shadow_caster_directional_light_index = 0, shadow_map_index = SHADOW_MAP_000_INDEX;
             shadow_caster_directional_light_index < GX_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER;
             ++shadow_caster_directional_light_index) {
            for (int cascaded_inex = 0; cascaded_inex < GX_MAX_SHADOW_CASCADES; ++cascaded_inex, ++shadow_map_index) {
                prs->set_directional_lights_shadow_map(
                    reinterpret_cast<texture::Texture2D*>(input_textures[shadow_map_index]),
                    shadow_caster_directional_light_index, cascaded_inex);
            }
        }
        kernel->secondary_cmd->bind(prs);
    }
}

gearoenix::render::graph::node::ForwardPbr::ForwardPbr(
    engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
    : Node(
        e,
        pipeline::Type::ForwardPbr,
        4 + GX_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER * GX_MAX_SHADOW_CASCADES,
        1,
        {
            "diffuse environment",
            "specular environment",
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
    auto* const txtmgr = e->get_system_application()->get_asset_manager()->get_texture_manager();
    core::sync::EndCaller<texture::Cube> txtcubecall([call](const std::shared_ptr<texture::Cube>&) {});
    core::sync::EndCaller<texture::Texture2D> txt2dcall([call](const std::shared_ptr<texture::Texture2D>&) {});

    input_textures[DIFFUSE_ENVIRONMENT_INDEX] = txtmgr->get_cube_zero_3c(txtcubecall).get();
    input_textures[SPECULAR_ENVIRONMENT_INDEX] = txtmgr->get_cube_zero_3c(txtcubecall).get();
    input_textures[AMBIENT_OCCLUSION_INDEX] = txtmgr->get_2d_one_1c(txt2dcall).get();
    input_textures[BRDFLUT_INDEX] = txtmgr->get_2d_one_2c(txt2dcall).get();
}

gearoenix::render::graph::node::ForwardPbr::~ForwardPbr() noexcept
{
    frames.clear();
}

void gearoenix::render::graph::node::ForwardPbr::set_diffuse_environment(texture::Cube* const t) noexcept
{
    set_input_texture(t, DIFFUSE_ENVIRONMENT_INDEX);
}

void gearoenix::render::graph::node::ForwardPbr::set_specular_environment(texture::Cube* const t) noexcept
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
    const unsigned int frame_number = e->get_frame_number();
    frame = frames[frame_number].get();
    for (auto& kernel : frame->kernels) {
        kernel->render_data_pool.refresh();
        kernel->secondary_cmd->begin();
    }
}

void gearoenix::render::graph::node::ForwardPbr::set_scene(const scene::Scene* const s) noexcept
{
    scn = s;
}

void gearoenix::render::graph::node::ForwardPbr::set_camera(const camera::Camera* const c) noexcept
{
    cam = c;
}

void gearoenix::render::graph::node::ForwardPbr::set_seen_models(const std::vector<model::Model*>* const ms) noexcept
{
    seen_models = ms;
}

void gearoenix::render::graph::node::ForwardPbr::set_directional_lights(const std::vector<std::pair<light::Directional*, light::CascadeInfo*>>* const m) noexcept
{
    directional_lights = m;
    unsigned int shadow_map_index = node::ForwardPbr::SHADOW_MAP_000_INDEX;
    int directional_light_index = 0;
    for (const auto& light_cascades_info : *m) {
        auto& cds = light_cascades_info.second->get_cascades_data();
        int cascaded_inex = 0;
        for (const light::CascadeInfo::PerCascade& c : cds) {
            graph::node::ShadowMapper* const shm = c.shadow_mapper.get();
            core::graph::Node::connect(shm, 0, this, shadow_map_index);
            input_textures[shadow_map_index] = shm->get_output_texture(0).get();
            ++cascaded_inex;
            ++shadow_map_index;
        }
        for (; cascaded_inex < GX_MAX_SHADOW_CASCADES; ++cascaded_inex, ++shadow_map_index) {
            clear_provider(shadow_map_index);
            input_textures[shadow_map_index] = nullptr;
        }
        ++directional_light_index;
    }
    for (; directional_light_index < GX_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER; ++directional_light_index) {
        for (int cascaded_inex = 0; cascaded_inex < GX_MAX_SHADOW_CASCADES; ++cascaded_inex, ++shadow_map_index) {
            clear_provider(shadow_map_index);
            input_textures[shadow_map_index] = nullptr;
        }
    }
}

void gearoenix::render::graph::node::ForwardPbr::record(const unsigned int kernel_index) noexcept
{
    const unsigned int kernels_count = e->get_kernels()->get_threads_count();
    unsigned int task_number = 0;

#define GX_DO_TASK(expr)                   \
    {                                      \
        if (task_number == kernel_index) { \
            expr;                          \
        }                                  \
        ++task_number;                     \
        task_number %= kernels_count;      \
    }

    for (auto* m : *seen_models) {
        GX_DO_TASK(record(m, kernel_index))
    }

#undef GX_DO_TASK
}

void gearoenix::render::graph::node::ForwardPbr::submit() noexcept
{
    const unsigned int frame_number = e->get_frame_number();
    command::Buffer* cmd = frames_primary_cmd[frame_number].get();
    cmd->bind(render_target.get());
    for (const auto& k : frame->kernels) {
        cmd->record(k->secondary_cmd.get());
    }
    Node::submit();
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
