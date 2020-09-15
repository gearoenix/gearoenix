#include "gx-gles2-pip-forward-pbr-resource-set.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../render/buffer/gx-rnd-buf-uniform.hpp"
#include "../../render/camera/gx-rnd-cmr-uniform.hpp"
#include "../../render/graph/node/gx-rnd-gr-nd-forward-pbr.hpp"
#include "../../render/material/gx-rnd-mat-pbr.hpp"
#include "../../render/mesh/gx-rnd-msh-mesh.hpp"
#include "../../render/scene/gx-rnd-scn-uniform.hpp"
#include "../buffer/gx-gles2-buf-index.hpp"
#include "../buffer/gx-gles2-buf-vertex.hpp"
#include "../shader/gx-gles2-shd-forward-pbr.hpp"
#include "../texture/gx-gles2-txt-2d.hpp"
#include "../texture/gx-gles2-txt-cube.hpp"
#include "../texture/gx-gles2-txt-texture.hpp"
#include "gx-gles2-pip-forward-pbr.hpp"
#include "gx-gles2-pip-resource-set.hpp"

gearoenix::gles2::pipeline::ForwardPbrResourceSet::ForwardPbrResourceSet(const std::shared_ptr<shader::ForwardPbr>& shd, std::shared_ptr<ForwardPbr const> pip) noexcept
    : render::pipeline::ForwardPbrResourceSet(std::move(pip))
    , base(new gles2::pipeline::ResourceSet(shd))
{
}

gearoenix::gles2::pipeline::ForwardPbrResourceSet::~ForwardPbrResourceSet() noexcept = default;

void gearoenix::gles2::pipeline::ForwardPbrResourceSet::bind_final(gl::uint& bound_shader_program) const noexcept
{
    GX_GLES2_PIP_RES_START_DRAWING_MESH
    GX_GLES2_PIP_RES_START_SHADER(ForwardPbr, shd)
    const auto* const camera = camera_uniform_buffer->get_ptr<render::camera::Uniform>();
    GX_GLES2_PIP_RES_SET_UNIFORM(camera_position, *(camera->position.data()))
    GX_GLES2_PIP_RES_SET_UNIFORM(camera_vp, camera->view_projection.data[0][0])
    GX_GLES2_PIP_RES_SET_UNIFORM(camera_hdr_tune_mapping, camera->hdr_tune_mapping)
    GX_GLES2_PIP_RES_SET_UNIFORM(camera_gamma_correction, camera->gamma_correction)
    //static_cast<const texture::Texture2D *>(ambient_occlusion.get())->bind(shdr->get_effect_ambient_occlusion_index());
    auto directional_lights_shadow_map_indices = shdr->get_effect_shadow_caster_directional_lights_cascades_shadow_map_indices();
    for (int i = 0, tii = 0; i < GX_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER; ++i) {
        for (int j = 0; j < GX_MAX_SHADOW_CASCADES; ++j, ++tii) {
            const render::texture::Texture2D* t = directional_lights_shadow_maps[i][j];
            if (t == nullptr)
                continue;
            const auto ti = static_cast<gl::enumerated>(directional_lights_shadow_map_indices[tii]);
            texture::Texture::bind(t, ti);
        }
    }
    const auto* const material = material_uniform_buffer->get_ptr<render::material::Pbr::Uniform>();
    GX_GLES2_PIP_RES_SET_UNIFORM(material_alpha, material->alpha)
    GX_GLES2_PIP_RES_SET_UNIFORM(material_alpha_cutoff, material->alpha_cutoff)
    GX_GLES2_PIP_RES_SET_TXT_2D(material_base_color, color)
    GX_GLES2_PIP_RES_SET_UNIFORM(material_emission_factor, *(material->emission_factor.data()))
    GX_GLES2_PIP_RES_SET_TXT_2D(material_emissive, emissive)
    GX_GLES2_PIP_RES_SET_UNIFORM(material_metallic_factor, material->metallic_factor)
    GX_GLES2_PIP_RES_SET_TXT_2D(material_metallic_roughness, metallic_roughness)
    GX_GLES2_PIP_RES_SET_TXT_2D(material_normal, normal)
    GX_GLES2_PIP_RES_SET_UNIFORM(material_normal_scale, material->normal_scale)
    //GX_GLES2_PIP_RES_SET_UNIFORM(material_occlusion_strength, material->occlusion_strength)
    GX_GLES2_PIP_RES_SET_UNIFORM(material_roughness_factor, material->roughness_factor)
    const auto* const model = model_uniform_buffer->get_ptr<math::Mat4x4<float>>();
    GX_GLES2_PIP_RES_SET_UNIFORM(model_m, model->data[0][0])
    const auto* const scene = scene_uniform_buffer->get_ptr<render::scene::Uniform>();
    GX_GLES2_PIP_RES_SET_UNIFORM(scene_ambient_light, *(scene->ambient_light.data()))
    GX_GLES2_PIP_RES_SET_UNIFORM(scene_directional_lights_color, *(scene->directional_lights_color[0].data()))
    GX_GLES2_PIP_RES_SET_UNIFORM(scene_directional_lights_direction, *(scene->directional_lights_direction[0].data()))
    GX_GLES2_PIP_RES_SET_UNIFORM(scene_directional_lights_count, *(scene->lights_count.data()))
    //shdr->set_scene_ssao_config_data(scene->ssao_config.data());
    const auto* const node = node_uniform_buffer->get_ptr<render::graph::node::ForwardPbrUniform>();
    GX_GLES2_PIP_RES_SET_UNIFORM(effect_point_lights_color_min_radius, *(node->point_lights_color_min_radius[0].data()))
    GX_GLES2_PIP_RES_SET_UNIFORM(effect_point_lights_count, node->point_lights_count)
    GX_GLES2_PIP_RES_SET_UNIFORM(effect_point_lights_position_max_radius, *(node->point_lights_position_max_radius[0].data()))
    GX_GLES2_PIP_RES_SET_UNIFORM(effect_shadow_caster_directional_lights_cascades_view_projection_bias, node->shadow_caster_directional_lights_cascades_view_projections_bias[0].data[0][0])
    GX_GLES2_PIP_RES_SET_UNIFORM(effect_shadow_caster_directional_lights_color_cascades_count, *(node->shadow_caster_directional_lights_color_cascades_count[0].data()))
    GX_GLES2_PIP_RES_SET_UNIFORM(effect_shadow_caster_directional_lights_count, node->shadow_caster_directional_lights_count)
    GX_GLES2_PIP_RES_SET_UNIFORM(effect_shadow_caster_directional_lights_direction, *(node->shadow_caster_directional_lights_direction[0].data()))
    GX_GLES2_PIP_RES_END_DRAWING_MESH
}

#endif