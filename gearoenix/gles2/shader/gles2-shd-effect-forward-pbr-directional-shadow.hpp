#ifndef GEAROENIX_GLES2_SHADER_FORWARD_PBR_DIRECTIONAL_SHADOW_HPP
#define GEAROENIX_GLES2_SHADER_FORWARD_PBR_DIRECTIONAL_SHADOW_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/sync/cr-sync-end-caller.hpp"
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
			};
		} // namespace shader
	} // namespace gles2
} // namespace gearoenix
#endif
#endif
