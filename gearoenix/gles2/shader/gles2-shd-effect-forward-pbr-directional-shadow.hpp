#ifndef GEAROENIX_GLES2_SHADER_FORWARD_PBR_DIRECTIONAL_SHADOW_HPP
#define GEAROENIX_GLES2_SHADER_FORWARD_PBR_DIRECTIONAL_SHADOW_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "gles2-shd-shader.hpp"
#include "../gles2.hpp"
#include <string>
namespace gearoenix {
namespace gles2 {
	namespace engine {
		class Engine;
	}
    namespace shader {
        class ForwardPbrDirectionalShadow : public Shader {
		private:
			GLuint camera_vp;
			GLuint light_vp_bias;
			GLuint model_m;
			GLuint material_alpha;
			GLuint material_alpha_cutoff;
			GLuint material_metallic_factor;
			GLuint material_normal_scale;
			GLuint material_occlusion_strength;
			GLuint material_roughness_factor;
			GLuint material_base_color;
			GLuint material_metallic_roughness;
			GLuint material_normal;
			GLuint material_emissive;
			GLuint scene_ambient_light;
			GLuint scene_directional_lights_color[GX_MAX_DIRECTIONAL_LIGHTS];
			GLuint scene_directional_lights_direction[GX_MAX_DIRECTIONAL_LIGHTS];
			GLuint scene_point_lights_color_min_radius[GX_MAX_POINT_LIGHTS];
			GLuint scene_point_lights_position_max_radius[GX_MAX_POINT_LIGHTS];
			GLuint scene_lights_count;
			GLuint scene_ssao_config;
			GLuint effect_diffuse_environment;
			GLuint effect_specular_environment;
			GLuint effect_ambient_occlusion;
			GLuint effect_shadow_map;
			GLuint effect_brdflut;
			GLuint light_color;
			GLuint light_direction;
			GLuint camera_position;
        public:
			ForwardPbrDirectionalShadow(const std::shared_ptr<engine::Engine> &e, const core::sync::EndCaller<core::sync::EndCallerIgnore> &c);
            ~ForwardPbrDirectionalShadow() override final;
        };
    }
}
}
#endif
#endif
