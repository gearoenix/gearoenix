#ifndef GEAROENIX_GLES2_SHADER_FORWARD_PBR_DIRECTIONAL_SHADOW_HPP
#define GEAROENIX_GLES2_SHADER_FORWARD_PBR_DIRECTIONAL_SHADOW_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../core/cr-static.hpp"
#include "gles2-shd-shader.hpp"
#include "../gles2.hpp"
#include <string>
namespace gearoenix
{
	namespace gles2
	{
		namespace engine
		{
			class Engine;
		}
		namespace shader
		{
			class ForwardPbrDirectionalShadow : public Shader
			{
			private:
				gl::uint camera_position = GX_SHADER_UNIFORM_FAILED;
				gl::uint camera_vp = GX_SHADER_UNIFORM_FAILED;
				gl::uint effect_ambient_occlusion = GX_SHADER_UNIFORM_FAILED;
				gl::uint effect_brdflut = GX_SHADER_UNIFORM_FAILED;
				gl::uint effect_diffuse_environment = GX_SHADER_UNIFORM_FAILED;
				gl::uint effect_shadow_map = GX_SHADER_UNIFORM_FAILED;
				gl::uint effect_specular_environment = GX_SHADER_UNIFORM_FAILED;
				gl::uint light_color = GX_SHADER_UNIFORM_FAILED;
				gl::uint light_direction = GX_SHADER_UNIFORM_FAILED;
				gl::uint light_vp_bias = GX_SHADER_UNIFORM_FAILED;
				gl::uint material_alpha = GX_SHADER_UNIFORM_FAILED;
				gl::uint material_alpha_cutoff = GX_SHADER_UNIFORM_FAILED;
				gl::uint material_base_color = GX_SHADER_UNIFORM_FAILED;
				gl::uint material_emissive = GX_SHADER_UNIFORM_FAILED;
				gl::uint material_metallic_factor = GX_SHADER_UNIFORM_FAILED;
				gl::uint material_metallic_roughness = GX_SHADER_UNIFORM_FAILED;
				gl::uint material_normal = GX_SHADER_UNIFORM_FAILED;
				gl::uint material_normal_scale = GX_SHADER_UNIFORM_FAILED;
				gl::uint material_occlusion_strength = GX_SHADER_UNIFORM_FAILED;
				gl::uint material_roughness_factor = GX_SHADER_UNIFORM_FAILED;
				gl::uint model_m = GX_SHADER_UNIFORM_FAILED;
				gl::uint scene_ambient_light = GX_SHADER_UNIFORM_FAILED;
				gl::uint scene_directional_lights_color = GX_SHADER_UNIFORM_FAILED;
				gl::uint scene_directional_lights_direction = GX_SHADER_UNIFORM_FAILED;
				gl::uint scene_lights_count = GX_SHADER_UNIFORM_FAILED;
				gl::uint scene_point_lights_color_min_radius = GX_SHADER_UNIFORM_FAILED;
				gl::uint scene_point_lights_position_max_radius = GX_SHADER_UNIFORM_FAILED;
				gl::uint scene_ssao_config = GX_SHADER_UNIFORM_FAILED;
				// Textures indices
				gl::sint material_base_color_index = -1;
				gl::sint material_metallic_roughness_index = -1;
				gl::sint material_normal_index = -1;
				gl::sint material_emissive_index = -1;
				gl::sint effect_diffuse_environment_index = -1;
				gl::sint effect_specular_environment_index = -1;
				gl::sint effect_ambient_occlusion_index = -1;
				gl::sint effect_shadow_map_index = -1;
				gl::sint effect_brdflut_index = -1;
			public:
				ForwardPbrDirectionalShadow(const std::shared_ptr<engine::Engine> &e, const core::sync::EndCaller<core::sync::EndCallerIgnore> &c);
				~ForwardPbrDirectionalShadow() override final;
				void bind() const override final;
				GX_GETTER_BUILDER(camera_position);
				GX_GETTER_BUILDER(camera_vp);
				GX_GETTER_BUILDER(effect_ambient_occlusion);
				GX_GETTER_BUILDER(effect_brdflut);
				GX_GETTER_BUILDER(effect_diffuse_environment);
				GX_GETTER_BUILDER(effect_shadow_map);
				GX_GETTER_BUILDER(effect_specular_environment);
				GX_GETTER_BUILDER(light_color);
				GX_GETTER_BUILDER(light_direction);
				GX_GETTER_BUILDER(light_vp_bias);
				GX_GETTER_BUILDER(material_alpha);
				GX_GETTER_BUILDER(material_alpha_cutoff);
				GX_GETTER_BUILDER(material_base_color);
				GX_GETTER_BUILDER(material_emissive);
				GX_GETTER_BUILDER(material_metallic_factor);
				GX_GETTER_BUILDER(material_metallic_roughness);
				GX_GETTER_BUILDER(material_normal);
				GX_GETTER_BUILDER(material_normal_scale);
				GX_GETTER_BUILDER(material_occlusion_strength);
				GX_GETTER_BUILDER(material_roughness_factor);
				GX_GETTER_BUILDER(model_m);
				GX_GETTER_BUILDER(scene_ambient_light);
				GX_GETTER_BUILDER(scene_directional_lights_color);
				GX_GETTER_BUILDER(scene_directional_lights_direction);
				GX_GETTER_BUILDER(scene_lights_count);
				GX_GETTER_BUILDER(scene_point_lights_color_min_radius);
				GX_GETTER_BUILDER(scene_point_lights_position_max_radius);
				GX_GETTER_BUILDER(scene_ssao_config);
				GX_GETTER_BUILDER(material_base_color_index);
				GX_GETTER_BUILDER(material_metallic_roughness_index);
				GX_GETTER_BUILDER(material_normal_index);
				GX_GETTER_BUILDER(material_emissive_index);
				GX_GETTER_BUILDER(effect_diffuse_environment_index);
				GX_GETTER_BUILDER(effect_specular_environment_index);
				GX_GETTER_BUILDER(effect_ambient_occlusion_index);
				GX_GETTER_BUILDER(effect_shadow_map_index);
				GX_GETTER_BUILDER(effect_brdflut_index);
#define GXHELPER(x) void set_##x##_data(const float *data) const;
				GXHELPER(camera_position);
				GXHELPER(camera_vp);
				GXHELPER(effect_ambient_occlusion);
				GXHELPER(effect_brdflut);
				GXHELPER(effect_diffuse_environment);
				GXHELPER(effect_shadow_map);
				GXHELPER(effect_specular_environment);
				GXHELPER(light_color);
				GXHELPER(light_direction);
				GXHELPER(light_vp_bias);
				GXHELPER(material_alpha);
				GXHELPER(material_alpha_cutoff);
				GXHELPER(material_base_color);
				GXHELPER(material_emissive);
				GXHELPER(material_metallic_factor);
				GXHELPER(material_metallic_roughness);
				GXHELPER(material_normal);
				GXHELPER(material_normal_scale);
				GXHELPER(material_occlusion_strength);
				GXHELPER(material_roughness_factor);
				GXHELPER(model_m);
				GXHELPER(scene_ambient_light);
				GXHELPER(scene_directional_lights_color);
				GXHELPER(scene_directional_lights_direction);
				GXHELPER(scene_lights_count);
				GXHELPER(scene_point_lights_color_min_radius);
				GXHELPER(scene_point_lights_position_max_radius);
				GXHELPER(scene_ssao_config);
				GXHELPER(material_base_color_index);
				GXHELPER(material_metallic_roughness_index);
				GXHELPER(material_normal_index);
				GXHELPER(material_emissive_index);
				GXHELPER(effect_diffuse_environment_index);
				GXHELPER(effect_specular_environment_index);
				GXHELPER(effect_ambient_occlusion_index);
				GXHELPER(effect_shadow_map_index);
				GXHELPER(effect_brdflut_index);
#undef GXHELPER
			};
		} // namespace shader
	} // namespace gles2
} // namespace gearoenix
#endif
#endif
