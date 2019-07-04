#ifndef GEAROENIX_GLES2_SHADER_FORWARD_PBR_DIRECTIONAL_SHADOW_HPP
#define GEAROENIX_GLES2_SHADER_FORWARD_PBR_DIRECTIONAL_SHADOW_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/cr-static.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../gles2.hpp"
#include "gles2-shd-shader.hpp"
#include <string>
namespace gearoenix::gles2 {
    namespace engine {
        class Engine;
    }
    namespace shader {
        class ForwardPbrDirectionalShadow : public Shader {
        private:
            GX_GLES2_SHADER_MATERIAL_UNIFORMS
            GX_GLES2_UNIFORM_VECTOR(camera_position, 3, 1)
            GX_GLES2_UNIFORM_MATRIX(camera_vp, 4, 1)
            GX_GLES2_UNIFORM_TEXTURE(effect_ambient_occlusion)
            GX_GLES2_UNIFORM_TEXTURE(effect_brdflut)
            GX_GLES2_UNIFORM_FLOAT(effect_cascades_count)
            GX_GLES2_UNIFORM_TEXTURE(effect_diffuse_environment)
            GX_GLES2_UNIFORM_TEXTURE(effect_shadow_map)
            GX_GLES2_UNIFORM_TEXTURE(effect_specular_environment)
            GX_GLES2_UNIFORM_MATRIX(effect_view_projection_biases, 4, GX_MAX_SHADOW_CASCADES)
            GX_GLES2_UNIFORM_VECTOR(light_color, 3, 1)
            GX_GLES2_UNIFORM_VECTOR(light_direction, 3, 1)
            GX_GLES2_UNIFORM_MATRIX(model_m, 4, 1)
            GX_GLES2_UNIFORM_VECTOR(scene_ambient_light, 3, 1)
            GX_GLES2_UNIFORM_VECTOR(scene_directional_lights_color, 4, GX_MAX_DIRECTIONAL_LIGHTS)
            GX_GLES2_UNIFORM_VECTOR(scene_directional_lights_direction, 4, GX_MAX_DIRECTIONAL_LIGHTS)
            GX_GLES2_UNIFORM_VECTOR(scene_lights_count, 2, 1)
            GX_GLES2_UNIFORM_VECTOR(scene_point_lights_color_min_radius, 4, GX_MAX_POINT_LIGHTS)
            GX_GLES2_UNIFORM_VECTOR(scene_point_lights_position_max_radius, 4, GX_MAX_POINT_LIGHTS)
            GX_GLES2_UNIFORM_VECTOR(scene_ssao_config, 3, 1)

        public:
            ForwardPbrDirectionalShadow(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
            ~ForwardPbrDirectionalShadow() noexcept final;
            void bind() const noexcept final;
        };
    }
}
#endif
#endif
