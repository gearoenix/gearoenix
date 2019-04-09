#include "gles2-pip-forward-pbr-directional-shadow-resource-set.hpp"
#include "../../render/camera/rnd-cmr-uniform.hpp"
#include "../../render/buffer/rnd-buf-uniform.hpp"
#include "../../render/light/rnd-lt-directional.hpp"
#include "../../render/material/rnd-mat-uniform.hpp"
#include "../shader/gles2-shd-effect-forward-pbr-directional-shadow.hpp"
#include "../texture/gles2-txt-2d.hpp"
#include "../texture/gles2-txt-cube.hpp"

gearoenix::gles2::pipeline::ForwardPbrDirectionalShadowResourceSet::ForwardPbrDirectionalShadowResourceSet(const std::shared_ptr<shader::ForwardPbrDirectionalShadow>& shd)
	: gles2::pipeline::ResourceSet(shd)
{
}

void gearoenix::gles2::pipeline::ForwardPbrDirectionalShadowResourceSet::bind(gl::uint &bound_shader_program) const
{
	gles2::pipeline::ResourceSet::bind(bound_shader_program);
	const shader::ForwardPbrDirectionalShadow *shdr = static_cast<const shader::ForwardPbrDirectionalShadow *>(shd.get());
	const render::camera::Uniform *camera = reinterpret_cast<const render::camera::Uniform *>(render::pipeline::ResourceSet::camera_uniform_buffer->get_data());
	shdr->set_camera_position_data(camera->position_far.data());
	shdr->set_camera_vp_data(camera->view_projection.data());
	static_cast<const texture::Texture2D *>(ambient_occlusion.get())->bind(shdr->get_effect_ambient_occlusion_index());
	static_cast<const texture::Texture2D *>(brdflut.get())->bind(shdr->get_effect_brdflut_index());
	static_cast<const texture::Cube *>(diffuse_environment.get())->bind(shdr->get_effect_diffuse_environment_index());
	static_cast<const texture::Texture2D *>(shadow_map.get())->bind(shdr->get_effect_shadow_map_index());
	static_cast<const texture::Cube *>(specular_environment.get())->bind(shdr->get_effect_specular_environment_index());
	const render::light::DirectionalUniform *light = reinterpret_cast<const render::light::DirectionalUniform *>(render::pipeline::ResourceSet::light_uniform_buffer->get_data());
	shdr->set_light_color_data(light->color_strength.data());
	shdr->set_light_direction_data(light->direction.data());
	shdr->set_light_vp_bias_data(light->vpbs[0].data());
	const render::material::Uniform *material = reinterpret_cast<const render::material::Uniform *>(render::pipeline::ResourceSet::material_uniform_buffer->get_data());
	shdr->set_material_alpha_data(&(material->alpha));
	shdr->set_material_alpha_cutoff_data(&(material->alpha_cutoff));
	//material_base_color
	//material_emissive
	shdr->set_material_metallic_factor_data(&(material->metallic_factor));
	//material_metallic_roughness
	//material_normal
	shdr->set_material_normal_scale_data(&(material->normal_scale));
	shdr->set_material_occlusion_strength_data(&(material->occlusion_strength));
	shdr->set_material_roughness_factor_data(&(material->roughness_factor));
	model_m
	scene_ambient_light
	scene_directional_lights_color
	scene_directional_lights_direction
	scene_lights_count
	scene_point_lights_color_min_radius
	scene_point_lights_position_max_radius
	scene_ssao_config
	GXUNIMPLEMENTED;
}
