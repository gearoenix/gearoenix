#ifndef GEAROENIX_GLES2_SHADER_FORWARD_PBR_DIRECTIONAL_SHADOW_HPP
#define GEAROENIX_GLES2_SHADER_FORWARD_PBR_DIRECTIONAL_SHADOW_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/cr-static.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../gles2.hpp"
#include "gles2-shd-shader.hpp"
#include <string>
namespace gearoenix {
namespace gles2 {
    namespace engine {
        class Engine;
    }
    namespace shader {
        class ForwardPbrDirectionalShadow : public Shader {
        private:
            GX_GLES2_SHADER_MATERIAL_UNIFORMS_LOCATIONS
            gl::sint camera_position = GX_GLES2_UNIFORM_FAILED;
            gl::sint camera_vp = GX_GLES2_UNIFORM_FAILED;
            gl::sint effect_ambient_occlusion = GX_GLES2_UNIFORM_FAILED;
            gl::sint effect_brdflut = GX_GLES2_UNIFORM_FAILED;
            gl::sint effect_diffuse_environment = GX_GLES2_UNIFORM_FAILED;
            gl::sint effect_shadow_map = GX_GLES2_UNIFORM_FAILED;
            gl::sint effect_specular_environment = GX_GLES2_UNIFORM_FAILED;
            gl::sint light_color = GX_GLES2_UNIFORM_FAILED;
            gl::sint light_direction = GX_GLES2_UNIFORM_FAILED;
            gl::sint light_vp_bias = GX_GLES2_UNIFORM_FAILED;
            gl::sint model_m = GX_GLES2_UNIFORM_FAILED;
            gl::sint scene_ambient_light = GX_GLES2_UNIFORM_FAILED;
            gl::sint scene_directional_lights_color = GX_GLES2_UNIFORM_FAILED;
            gl::sint scene_directional_lights_direction = GX_GLES2_UNIFORM_FAILED;
            gl::sint scene_lights_count = GX_GLES2_UNIFORM_FAILED;
            gl::sint scene_point_lights_color_min_radius = GX_GLES2_UNIFORM_FAILED;
            gl::sint scene_point_lights_position_max_radius = GX_GLES2_UNIFORM_FAILED;
            gl::sint scene_ssao_config = GX_GLES2_UNIFORM_FAILED;
            gl::sint effect_diffuse_environment_index = -1;
            gl::sint effect_specular_environment_index = -1;
            gl::sint effect_ambient_occlusion_index = -1;
            gl::sint effect_shadow_map_index = -1;
            gl::sint effect_brdflut_index = -1;

        public:
            ForwardPbrDirectionalShadow(const std::shared_ptr<engine::Engine>& e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c);
            ~ForwardPbrDirectionalShadow() override final;
            void bind() const override final;
            GX_GLES2_SHADER_MATERIAL_FUNCTION_DECLARATIONS
            GX_GETTER_BUILDER(camera_position)
            GX_GETTER_BUILDER(camera_vp)
            GX_GETTER_BUILDER(effect_ambient_occlusion)
            GX_GETTER_BUILDER(effect_brdflut)
            GX_GETTER_BUILDER(effect_diffuse_environment)
            GX_GETTER_BUILDER(effect_shadow_map)
            GX_GETTER_BUILDER(effect_specular_environment)
            GX_GETTER_BUILDER(light_color)
            GX_GETTER_BUILDER(light_direction)
            GX_GETTER_BUILDER(light_vp_bias)
            GX_GETTER_BUILDER(model_m)
            GX_GETTER_BUILDER(scene_ambient_light)
            GX_GETTER_BUILDER(scene_directional_lights_color)
            GX_GETTER_BUILDER(scene_directional_lights_direction)
            GX_GETTER_BUILDER(scene_lights_count)
            GX_GETTER_BUILDER(scene_point_lights_color_min_radius)
            GX_GETTER_BUILDER(scene_point_lights_position_max_radius)
            GX_GETTER_BUILDER(scene_ssao_config)
            GX_GETTER_BUILDER(effect_diffuse_environment_index)
            GX_GETTER_BUILDER(effect_specular_environment_index)
            GX_GETTER_BUILDER(effect_ambient_occlusion_index)
            GX_GETTER_BUILDER(effect_shadow_map_index)
            GX_GETTER_BUILDER(effect_brdflut_index)
#define GX_HELPER(x) void set_##x##_data(const float* data) const;
            GX_HELPER(camera_position)
            GX_HELPER(camera_vp)
            GX_HELPER(effect_ambient_occlusion)
            GX_HELPER(effect_brdflut)
            GX_HELPER(effect_diffuse_environment)
            GX_HELPER(effect_shadow_map)
            GX_HELPER(effect_specular_environment)
            GX_HELPER(light_color)
            GX_HELPER(light_direction)
            GX_HELPER(light_vp_bias)
            GX_HELPER(model_m)
            GX_HELPER(scene_ambient_light)
            GX_HELPER(scene_directional_lights_color)
            GX_HELPER(scene_directional_lights_direction)
            GX_HELPER(scene_lights_count)
            GX_HELPER(scene_point_lights_color_min_radius)
            GX_HELPER(scene_point_lights_position_max_radius)
            GX_HELPER(scene_ssao_config)
            GX_HELPER(effect_diffuse_environment_index)
            GX_HELPER(effect_specular_environment_index)
            GX_HELPER(effect_ambient_occlusion_index)
            GX_HELPER(effect_shadow_map_index)
            GX_HELPER(effect_brdflut_index)
#undef GX_HELPER
        };
    } // namespace shader
} // namespace gles2
} // namespace gearoenix
#endif
#endif
