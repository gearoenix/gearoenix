#ifndef GEAROENIX_GLC3_SHADER_FORWARD_PBR_HPP
#define GEAROENIX_GLC3_SHADER_FORWARD_PBR_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/cr-static.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../glc3.hpp"
#include "glc3-shd-shader.hpp"
#include <string>

namespace gearoenix::glc3 {
namespace engine {
    class Engine;
}
namespace shader {
    class ForwardPbr : public Shader {
    private:
        GX_GLC3_SHADER_MATERIAL_UNIFORMS
        GX_GLC3_UNIFORM_VECTOR(camera_position, 3, 1)
        GX_GLC3_UNIFORM_MATRIX(camera_vp, 4, 1)
        GX_GLC3_UNIFORM_TEXTURE(effect_ambient_occlusion)
        GX_GLC3_UNIFORM_TEXTURE(effect_brdflut)
        GX_GLC3_UNIFORM_TEXTURE(effect_diffuse_environment)
        GX_GLC3_UNIFORM_FLOAT(effect_directional_lights_cascades_count, GX_MAX_DIRECTIONAL_LIGHTS)
        GX_GLC3_UNIFORM_TEXTURE_ARRAY(effect_directional_lights_cascades_shadow_map, GX_MAX_SHADOW_CASCADES* GX_MAX_DIRECTIONAL_LIGHTS)
        GX_GLC3_UNIFORM_MATRIX(effect_directional_lights_cascades_view_projection_bias, 4, GX_MAX_SHADOW_CASCADES* GX_MAX_DIRECTIONAL_LIGHTS)
        GX_GLC3_UNIFORM_VECTOR(effect_directional_lights_color, 3, GX_MAX_DIRECTIONAL_LIGHTS)
        GX_GLC3_UNIFORM_FLOAT(effect_directional_lights_count, 1)
        GX_GLC3_UNIFORM_VECTOR(effect_directional_lights_direction, 3, GX_MAX_DIRECTIONAL_LIGHTS)
        GX_GLC3_UNIFORM_TEXTURE(effect_specular_environment)
        GX_GLC3_UNIFORM_MATRIX(model_m, 4, 1)
        GX_GLC3_UNIFORM_VECTOR(scene_ambient_light, 3, 1)
        GX_GLC3_UNIFORM_VECTOR(scene_directional_lights_color, 4, GX_MAX_DIRECTIONAL_LIGHTS)
        GX_GLC3_UNIFORM_VECTOR(scene_directional_lights_direction, 4, GX_MAX_DIRECTIONAL_LIGHTS)
        GX_GLC3_UNIFORM_VECTOR(scene_lights_count, 2, 1)
        GX_GLC3_UNIFORM_VECTOR(scene_point_lights_color_min_radius, 4, GX_MAX_POINT_LIGHTS)
        GX_GLC3_UNIFORM_VECTOR(scene_point_lights_position_max_radius, 4, GX_MAX_POINT_LIGHTS)
        GX_GLC3_UNIFORM_VECTOR(scene_ssao_config, 3, 1)

    public:
        ForwardPbr(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
        ~ForwardPbr() noexcept final;
        void bind() const noexcept final;
    };
}
}
#endif
#endif
