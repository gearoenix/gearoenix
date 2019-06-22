#include "gles2-pip-forward-pbr-directional-shadow-resource-set.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../render/graph/node/rnd-gr-nd-forward-pbr-directional-shadow.hpp"
#include "../../render/buffer/rnd-buf-uniform.hpp"
#include "../../render/camera/rnd-cmr-uniform.hpp"
#include "../../render/light/rnd-lt-directional.hpp"
#include "../../render/material/rnd-mat-uniform.hpp"
#include "../../render/mesh/rnd-msh-mesh.hpp"
#include "../../render/model/rnd-mdl-uniform.hpp"
#include "../../render/scene/rnd-scn-uniform.hpp"
#include "../buffer/gles2-buf-index.hpp"
#include "../buffer/gles2-buf-vertex.hpp"
#include "../gles2.hpp"
#include "../shader/gles2-shd-effect-forward-pbr-directional-shadow.hpp"
#include "../texture/gles2-txt-2d.hpp"
#include "../texture/gles2-txt-cube.hpp"

gearoenix::gles2::pipeline::ForwardPbrDirectionalShadowResourceSet::ForwardPbrDirectionalShadowResourceSet(const std::shared_ptr<shader::ForwardPbrDirectionalShadow>& shd)
    : gles2::pipeline::ResourceSet(shd)
{
}

void gearoenix::gles2::pipeline::ForwardPbrDirectionalShadowResourceSet::bind(gl::uint& bound_shader_program) const
{
	GX_GLES2_PIPRES_START_DRAWING_MESH
	GX_GLES2_PIPRES_START_SHADER(ForwardPbrDirectionalShadow, shd)
    auto camera = reinterpret_cast<const render::camera::Uniform*>(camera_uniform_buffer->get_data());
    shdr->set_camera_position_data(camera->position.data());
    shdr->set_camera_vp_data(camera->view_projection.data());
    //static_cast<const texture::Texture2D *>(ambient_occlusion.get())->bind(shdr->get_effect_ambient_occlusion_index());
    reinterpret_cast<const texture::Texture2D*>(brdflut)->bind(static_cast<gl::enumerated>(shdr->get_effect_brdflut_index()));
    reinterpret_cast<const texture::Cube*>(diffuse_environment)->bind(static_cast<gl::enumerated>(shdr->get_effect_diffuse_environment_index()));
    reinterpret_cast<const texture::Texture2D*>(shadow_map)->bind(shdr->get_effect_shadow_map_index());
    reinterpret_cast<const texture::Cube*>(specular_environment)->bind(static_cast<gl::enumerated>(shdr->get_effect_specular_environment_index()));
    const auto* light = reinterpret_cast<const render::light::DirectionalUniform*>(light_uniform_buffer->get_data());
    shdr->set_light_color_data(light->color.data());
    shdr->set_light_direction_data(light->direction.data());
	GX_GLES2_PIPRES_BIND_MATERIAL
    auto model = reinterpret_cast<const render::model::Uniform*>(model_uniform_buffer->get_data());
    shdr->set_model_m_data(model->m.data());
    auto scene = reinterpret_cast<const render::scene::Uniform*>(scene_uniform_buffer->get_data());
    //shdr->set_scene_ambient_light_data(scene->ambient_light.data());
    shdr->set_scene_directional_lights_color_data(scene->directional_lights_color[0].data());
    shdr->set_scene_directional_lights_direction_data(scene->directional_lights_direction[0].data());
    shdr->set_scene_lights_count_data(scene->lights_count.data());
    shdr->set_scene_point_lights_color_min_radius_data(scene->point_lights_color_min_radius[0].data());
    shdr->set_scene_point_lights_position_max_radius_data(scene->point_lights_position_max_radius[0].data());
    //shdr->set_scene_ssao_config_data(scene->ssao_config.data());
	auto node = reinterpret_cast<const render::graph::node::ForwardPbrDirectionalShadowUniform*>(node_uniform_buffer->get_data());
	GX_GLES2_PIPRES_SET_UNIFORM(effect_view_projection_biases, *(node->cascades_view_projections_bias[0].data()))
	GX_GLES2_PIPRES_SET_UNIFORM(effect_cascades_count, node->cascades_count)
	GX_GLES2_PIPRES_END_DRAWING_MESH
}
