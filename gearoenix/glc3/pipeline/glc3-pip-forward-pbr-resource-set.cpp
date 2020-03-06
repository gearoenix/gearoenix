#include "glc3-pip-forward-pbr-resource-set.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../gl/gl-loader.hpp"
#include "../../render/buffer/rnd-buf-uniform.hpp"
#include "../../render/camera/rnd-cmr-uniform.hpp"
#include "../../render/graph/node/rnd-gr-nd-forward-pbr.hpp"
#include "../../render/light/rnd-lt-directional.hpp"
#include "../../render/material/rnd-mat-pbr.hpp"
#include "../../render/mesh/rnd-msh-mesh.hpp"
#include "../../render/scene/rnd-scn-uniform.hpp"
#include "../buffer/glc3-buf-index.hpp"
#include "../buffer/glc3-buf-vertex.hpp"
#include "../shader/glc3-shd-forward-pbr.hpp"
#include "../texture/glc3-txt-2d.hpp"
#include "../texture/glc3-txt-cube.hpp"
#include "../texture/glc3-txt-texture.hpp"
#include "glc3-pip-forward-pbr.hpp"
#include "glc3-pip-resource-set.hpp"

gearoenix::glc3::pipeline::ForwardPbrResourceSet::ForwardPbrResourceSet(const std::shared_ptr<shader::ForwardPbr>& shd, std::shared_ptr<ForwardPbr const> pip) noexcept
    : render::pipeline::ForwardPbrResourceSet(std::move(pip))
    , base(new glc3::pipeline::ResourceSet(shd))
{
}

gearoenix::glc3::pipeline::ForwardPbrResourceSet::~ForwardPbrResourceSet() noexcept = default;

void gearoenix::glc3::pipeline::ForwardPbrResourceSet::bind_final(gl::uint& bound_shader_program) const noexcept
{
    GX_GLC3_PIP_RES_START_DRAWING_MESH
    GX_GLC3_PIP_RES_START_SHADER(ForwardPbr, shd)
    const auto* const camera = camera_uniform_buffer->get_ptr<render::camera::Uniform>();
    GX_GLC3_PIP_RES_SET_UNIFORM(camera_position, *(camera->position.data()))
    GX_GLC3_PIP_RES_SET_UNIFORM(camera_vp, camera->view_projection.data[0][0])
    //static_cast<const texture::Texture2D *>(ambient_occlusion.get())->bind(shdr->get_effect_ambient_occlusion_index());
    reinterpret_cast<const texture::Texture2D*>(brdflut)->bind(static_cast<gl::enumerated>(shdr->get_effect_brdflut_index()));
    reinterpret_cast<const texture::TextureCube*>(diffuse_environment)->bind(static_cast<gl::enumerated>(shdr->get_effect_diffuse_environment_index()));
    auto directional_lights_shadow_map_indices = shdr->get_effect_shadow_caster_directional_lights_cascades_shadow_map_indices();
    for (int i = 0, tii = 0; i < GX_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER; ++i) {
        for (int j = 0; j < GX_MAX_SHADOW_CASCADES; ++j, ++tii) {
            const render::texture::Texture2D* const t = directional_lights_shadow_maps[i][j];
            if (t == nullptr)
                continue;
            const auto ti = static_cast<gl::enumerated>(directional_lights_shadow_map_indices[tii]);
            texture::Texture::bind(t, ti);
        }
    }
    reinterpret_cast<const texture::TextureCube*>(specular_environment)->bind(static_cast<gl::enumerated>(shdr->get_effect_specular_environment_index()));
    const auto* const material = material_uniform_buffer->get_ptr<render::material::Pbr::Uniform>();
    GX_GLC3_PIP_RES_SET_UNIFORM(material_alpha, material->alpha)
    GX_GLC3_PIP_RES_SET_UNIFORM(material_alpha_cutoff, material->alpha_cutoff)
    GX_GLC3_PIP_RES_SET_TXT_2D(material_base_color, color)
    //GX_GLC3_PIP_RES_SET_TXT_2D(material_emissive, emissive)
    GX_GLC3_PIP_RES_SET_UNIFORM(material_metallic_factor, material->metallic_factor)
    GX_GLC3_PIP_RES_SET_TXT_2D(material_metallic_roughness, metallic_roughness)
    GX_GLC3_PIP_RES_SET_TXT_2D(material_normal, normal)
    GX_GLC3_PIP_RES_SET_UNIFORM(material_normal_scale, material->normal_scale)
    GX_GLC3_PIP_RES_SET_UNIFORM(material_occlusion_strength, material->occlusion_strength)
    GX_GLC3_PIP_RES_SET_UNIFORM(material_roughness_factor, material->roughness_factor)
    const auto* const model = model_uniform_buffer->get_ptr<math::Mat4x4<float>>();
    GX_GLC3_PIP_RES_SET_UNIFORM(model_m, model->data[0][0])
    const auto* const scene = scene_uniform_buffer->get_ptr<render::scene::Uniform>();
    GX_GLC3_PIP_RES_SET_UNIFORM(scene_ambient_light, *(scene->ambient_light.data()))
    GX_GLC3_PIP_RES_SET_UNIFORM(scene_directional_lights_color, *(scene->directional_lights_color[0].data()))
    GX_GLC3_PIP_RES_SET_UNIFORM(scene_directional_lights_direction, *(scene->directional_lights_direction[0].data()))
    GX_GLC3_PIP_RES_SET_UNIFORM(scene_lights_count, *(scene->lights_count.data()))
    //shdr->set_scene_ssao_config_data(scene->ssao_config.data());
    const auto* const node = node_uniform_buffer->get_ptr<render::graph::node::ForwardPbrUniform>();
    GX_GLC3_PIP_RES_SET_UNIFORM(effect_point_lights_color_min_radius, *(node->point_lights_color_min_radius[0].data()))
    GX_GLC3_PIP_RES_SET_UNIFORM(effect_point_lights_count, node->point_lights_count)
    GX_GLC3_PIP_RES_SET_UNIFORM(effect_point_lights_position_max_radius, *(node->point_lights_position_max_radius[0].data()))
    GX_GLC3_PIP_RES_SET_UNIFORM(effect_shadow_caster_directional_lights_cascades_view_projection_bias, node->shadow_caster_directional_lights_cascades_view_projections_bias[0].data[0][0])
    GX_GLC3_PIP_RES_SET_UNIFORM(effect_shadow_caster_directional_lights_color_cascades_count, *(node->shadow_caster_directional_lights_color_cascades_count[0].data()))
    GX_GLC3_PIP_RES_SET_UNIFORM(effect_shadow_caster_directional_lights_count, node->shadow_caster_directional_lights_count)
    GX_GLC3_PIP_RES_SET_UNIFORM(effect_shadow_caster_directional_lights_direction, *(node->shadow_caster_directional_lights_direction[0].data()))
    GX_GLC3_PIP_RES_END_DRAWING_MESH
}

#endif