#include "gles2-pip-forward-pbr-directional-shadow-resource-set.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../render/camera/rnd-cmr-uniform.hpp"
#include "../../render/buffer/rnd-buf-uniform.hpp"
#include "../../render/light/rnd-lt-directional.hpp"
#include "../../render/material/rnd-mat-uniform.hpp"
#include "../../render/mesh/rnd-msh-mesh.hpp"
#include "../../render/model/rnd-mdl-uniform.hpp"
#include "../../render/scene/rnd-scn-uniform.hpp"
#include "../buffer/gles2-buf-index.hpp"
#include "../buffer/gles2-buf-vertex.hpp"
#include "../shader/gles2-shd-effect-forward-pbr-directional-shadow.hpp"
#include "../texture/gles2-txt-2d.hpp"
#include "../texture/gles2-txt-cube.hpp"

gearoenix::gles2::pipeline::ForwardPbrDirectionalShadowResourceSet::ForwardPbrDirectionalShadowResourceSet(const std::shared_ptr<shader::ForwardPbrDirectionalShadow>& shd)
	: gles2::pipeline::ResourceSet(shd)
{
}

void gearoenix::gles2::pipeline::ForwardPbrDirectionalShadowResourceSet::bind(gl::uint &bound_shader_program) const
{
	static_cast<buffer::Index *>(msh->get_index_buffer().get())->bind();
	static_cast<buffer::Vertex *>(msh->get_vertex_buffer().get())->bind();
	gles2::pipeline::ResourceSet::bind(bound_shader_program);
	const shader::ForwardPbrDirectionalShadow *shdr = static_cast<const shader::ForwardPbrDirectionalShadow *>(shd.get());
	const render::camera::Uniform *camera = reinterpret_cast<const render::camera::Uniform *>(camera_uniform_buffer->get_data());
	shdr->set_camera_position_data(camera->position_far.data());
	shdr->set_camera_vp_data(camera->view_projection.data());
	//static_cast<const texture::Texture2D *>(ambient_occlusion.get())->bind(shdr->get_effect_ambient_occlusion_index());
	static_cast<const texture::Texture2D *>(brdflut.get())->bind(shdr->get_effect_brdflut_index());
	static_cast<const texture::Cube *>(diffuse_environment.get())->bind(shdr->get_effect_diffuse_environment_index());
	//static_cast<const texture::Texture2D *>(shadow_map.get())->bind(shdr->get_effect_shadow_map_index());
	static_cast<const texture::Cube *>(specular_environment.get())->bind(shdr->get_effect_specular_environment_index());
	//const render::light::DirectionalUniform *light = reinterpret_cast<const render::light::DirectionalUniform *>(light_uniform_buffer->get_data());
	//shdr->set_light_color_data(light->color_strength.data());
	//shdr->set_light_direction_data(light->direction.data());
	//shdr->set_light_vp_bias_data(light->vpbs[0].data());
	const render::material::Uniform *material = reinterpret_cast<const render::material::Uniform *>(material_uniform_buffer->get_data());
	shdr->set_material_alpha_data(&(material->alpha));
	shdr->set_material_alpha_cutoff_data(&(material->alpha_cutoff));
	static_cast<const texture::Texture2D *>(color.get())->bind(shdr->get_material_base_color_index());
	//static_cast<const texture::Texture2D *>(emissive.get())->bind(shdr->get_material_emissive_index());
	shdr->set_material_metallic_factor_data(&(material->metallic_factor));
	static_cast<const texture::Texture2D *>(metallic_roughness.get())->bind(shdr->get_material_metallic_roughness_index());
	static_cast<const texture::Texture2D *>(normal.get())->bind(shdr->get_material_normal_index());
	shdr->set_material_normal_scale_data(&(material->normal_scale));
	//shdr->set_material_occlusion_strength_data(&(material->occlusion_strength));
	shdr->set_material_roughness_factor_data(&(material->roughness_factor));
	const render::model::Uniform *model = reinterpret_cast<const render::model::Uniform *>(model_uniform_buffer->get_data());
	shdr->set_model_m_data(model->m.data());
	const render::scene::Uniform *scene = reinterpret_cast<const render::scene::Uniform *>(scene_uniform_buffer->get_data());
	//shdr->set_scene_ambient_light_data(scene->ambient_light.data());
	//shdr->set_scene_directional_lights_color_data(scene->directional_lights_color[0].data());
	//shdr->set_scene_directional_lights_direction_data(scene->directional_lights_direction[0].data());
	//shdr->set_scene_lights_count_data(scene->lights_count.data());
	shdr->set_scene_point_lights_color_min_radius_data(scene->point_lights_color_min_radius[0].data());
	shdr->set_scene_point_lights_position_max_radius_data(scene->point_lights_position_max_radius[0].data());
	//shdr->set_scene_ssao_config_data(scene->ssao_config.data());
	static_cast<buffer::Index *>(msh->get_index_buffer().get())->draw();
}
