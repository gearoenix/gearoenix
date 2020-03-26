#ifndef GEAROENIX_GLC3_SHADER_FORWARD_PBR_HPP
#define GEAROENIX_GLC3_SHADER_FORWARD_PBR_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/cr-static.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../glc3.hpp"
#include "glc3-shd-shader.hpp"
#include <string>

namespace gearoenix::glc3::engine {
class Engine;
}

namespace gearoenix::glc3::shader {
class ForwardPbr final : public Shader {
    GX_GLC3_UNIFORM_FLOAT(material_alpha, 1)
    GX_GLC3_UNIFORM_FLOAT(material_alpha_cutoff, 1)
    GX_GLC3_UNIFORM_TEXTURE(material_base_color)
    GX_GLC3_UNIFORM_TEXTURE(material_emissive)
    GX_GLC3_UNIFORM_FLOAT(material_metallic_factor, 1)
    GX_GLC3_UNIFORM_TEXTURE(material_metallic_roughness)
    GX_GLC3_UNIFORM_TEXTURE(material_normal)
    GX_GLC3_UNIFORM_FLOAT(material_normal_scale, 1)
    GX_GLC3_UNIFORM_FLOAT(material_occlusion_strength, 1)
    GX_GLC3_UNIFORM_FLOAT(material_roughness_factor, 1)
    GX_GLC3_UNIFORM_VECTOR(camera_position, 3, 1)
    GX_GLC3_UNIFORM_MATRIX(camera_vp, 4, 1)
    GX_GLC3_UNIFORM_FLOAT(camera_hdr_tune_mapping, 1)
    GX_GLC3_UNIFORM_FLOAT(camera_gamma_correction, 1)
    // Effect textures
    GX_GLC3_UNIFORM_TEXTURE(effect_ambient_occlusion)
    GX_GLC3_UNIFORM_TEXTURE(effect_brdflut)
    GX_GLC3_UNIFORM_TEXTURE(effect_diffuse_environment)
    GX_GLC3_UNIFORM_TEXTURE(effect_specular_environment)
    GX_GLC3_UNIFORM_TEXTURE_ARRAY(effect_shadow_caster_directional_lights_cascades_shadow_map, GX_MAX_SHADOW_CASCADES* GX_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER)
    // Effect uniforms
    GX_GLC3_UNIFORM_VECTOR(effect_point_lights_color_min_radius, 4, GX_MAX_POINT_LIGHTS)
    GX_GLC3_UNIFORM_VECTOR(effect_point_lights_position_max_radius, 4, GX_MAX_POINT_LIGHTS)
    GX_GLC3_UNIFORM_MATRIX(effect_shadow_caster_directional_lights_cascades_view_projection_bias, 4, GX_MAX_SHADOW_CASCADES* GX_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER)
    GX_GLC3_UNIFORM_VECTOR(effect_shadow_caster_directional_lights_color_cascades_count, 4, GX_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER)
    GX_GLC3_UNIFORM_VECTOR(effect_shadow_caster_directional_lights_direction, 4, GX_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER)
    GX_GLC3_UNIFORM_FLOAT(effect_point_lights_count, 1)
    GX_GLC3_UNIFORM_FLOAT(effect_shadow_caster_directional_lights_count, 1)
    // Model
    GX_GLC3_UNIFORM_MATRIX(model_m, 4, 1)
    // Scene
    GX_GLC3_UNIFORM_VECTOR(scene_ambient_light, 3, 1)
    GX_GLC3_UNIFORM_VECTOR(scene_directional_lights_color, 4, GX_MAX_DIRECTIONAL_LIGHTS)
    GX_GLC3_UNIFORM_VECTOR(scene_directional_lights_direction, 4, GX_MAX_DIRECTIONAL_LIGHTS)
    GX_GLC3_UNIFORM_VECTOR(scene_lights_count, 2, 1)
    GX_GLC3_UNIFORM_VECTOR(scene_ssao_config, 3, 1)

public:
    ForwardPbr(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    ~ForwardPbr() noexcept final;
    void bind() const noexcept final;
};
}

#endif
#endif
