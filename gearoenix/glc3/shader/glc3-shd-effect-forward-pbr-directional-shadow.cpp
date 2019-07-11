#include "glc3-shd-effect-forward-pbr-directional-shadow.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../system/sys-log.hpp"
#include "../engine/glc3-eng-engine.hpp"
#include <sstream>

gearoenix::glc3::shader::ForwardPbrDirectionalShadow::ForwardPbrDirectionalShadow(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Shader(e, c)
{
    GX_GLC3_SHADER_SRC_DEFAULT_VERTEX_STARTING <<
        // camera uniform(s)
        "uniform mat4  camera_vp;\n"
        // effect uniform(s)
        "uniform mat4  effect_view_projection_biases[" GX_MAX_SHADOW_CASCADES_STR "];\n"
        "uniform float effect_cascades_count;\n"
        // light uniform(s)
        "uniform vec3  light_direction;\n"
        // model uniform(s)
        "uniform mat4  model_m;\n"
        // output(s)
        "out vec3 out_pos;\n"
        "out vec3 out_nrm;\n"
        "out vec3 out_tng;\n"
        "out vec3 out_btg;\n"
        "out vec2 out_uv;\n"
        "out vec3 out_light_poses[" GX_MAX_SHADOW_CASCADES_STR "];\n"
        // Main function
        "void main()\n"
        "{\n"
        "    vec4 pos = model_m * vec4(position, 1.0);\n"
        "    out_pos = pos.xyz;\n"
        "    out_nrm = normalize((model_m * vec4(normal, 0.0)).xyz);\n"
        "    out_tng = normalize((model_m * vec4(tangent.xyz, 0.0)).xyz);\n"
        "    out_btg = cross(out_nrm, out_tng) * tangent.w;\n"
        "    out_uv = uv;\n"
        "    int effect_cascades_count_int = int(effect_cascades_count);\n"
        "    for(int i = 0; i < effect_cascades_count_int; ++i)\n"
        "    {\n"
        "        vec4 light_pos = effect_view_projection_biases[i] * pos;\n"
        "        light_pos.xyz /= light_pos.w;\n"
        "        light_pos.z *= 0.5;\n"
        "        light_pos.z += 0.5;\n"
        "        out_light_poses[i] = light_pos.xyz;\n"
        //		"        if(light_pos.x > 0.0 && light_pos.x < 1.0 && light_pos.y > 0.0 && light_pos.y < 1.0)\n"
        //		"            break;\n"
        "    }\n"
        "    gl_Position = camera_vp * pos;\n"
        "}";
    GX_GLC3_SHADER_SRC_DEFAULT_FRAGMENT_STARTING << GX_GLC3_SHADER_SRC_MATERIAL_RESOURCES
        // scenes uniform(s)
        "uniform vec3        scene_ambient_light;\n"
        "uniform vec4        scene_directional_lights_color[" GX_MAX_DIRECTIONAL_LIGHTS_STR "];\n"
        "uniform vec4        scene_directional_lights_direction[" GX_MAX_DIRECTIONAL_LIGHTS_STR "];\n"
        "uniform vec4        scene_point_lights_color_min_radius[" GX_MAX_POINT_LIGHTS_STR "];\n"
        "uniform vec4        scene_point_lights_position_max_radius[" GX_MAX_POINT_LIGHTS_STR "];\n"
        // directional, point
        "uniform vec2        scene_lights_count;\n"
        // samples-count, radius, z-tolerance
        "uniform vec3        scene_ssao_config;\n"
        // effect uniforms
        "uniform samplerCube effect_diffuse_environment;\n"
        "uniform samplerCube effect_specular_environment;\n"
        "uniform sampler2D   effect_ambient_occlusion;\n"
        //        "uniform sampler2DShadow    effect_shadow_map;\n"
        "uniform sampler2D   effect_shadow_map;\n"
        "uniform sampler2D   effect_brdflut;\n"
        "uniform float       effect_cascades_count;\n"
        // light uniform(s)
        "uniform vec3        light_color;\n"
        "uniform vec3        light_direction;\n"
        // camera uniform(s)
        "uniform vec3        camera_position;\n"
        // output(s) of vertex shader
        "in vec3 out_pos;\n"
        "in vec3 out_nrm;\n"
        "in vec3 out_tng;\n"
        "in vec3 out_btg;\n"
        "in vec2 out_uv;\n"
        "in vec3 out_light_poses[" GX_MAX_SHADOW_CASCADES_STR "];\n"
        "out vec4 frag_color;\n"
        // Normal Distribution Function Trowbridge-Reitz GGX
        "float distribution_ggx(const vec3 normal, const vec3 halfway, const float roughness) {\n"
        "    float roughness2 = roughness * roughness;\n"
        "    float nh = max(dot(normal, halfway), 0.0);\n"
        "    float nh2 = nh * nh;\n"
        "    float nom = roughness2;\n"
        "    float tmpdenom = nh2 * (roughness2 - 1.0) + 1.0;\n"
        "    float denom = GXPI * tmpdenom * tmpdenom;\n"
        "    return nom / denom;\n"
        "}\n"
        "float geometry_schlick_ggx(const float nd, const float roughness, const float k, const float k_inv) {\n"
        "    float nom = nd;\n"
        "    float denom = nd * k_inv + k;\n"
        "    return nom / denom;\n"
        "}\n"
        "float geometry_smith(const float normal_dot_light, const float normal_dot_view, const float roughness) {\n"
        "    float k = roughness + 1.0;\n"
        "    k = (k * k) * (1.0 / 8.0);\n"
        "    float k_inv = 1.0 - k;\n"
        "    float ggx2 = geometry_schlick_ggx(normal_dot_view, roughness, k, k_inv);\n"
        "    float ggx1 = geometry_schlick_ggx(normal_dot_light, roughness, k, k_inv);\n"
        "    return ggx1 * ggx2;\n"
        "}\n"
        "vec3 fresnel_schlick(const float nv, const vec3 f0) {\n"
        "    float inv = 1.0 - nv;\n"
        "    float inv2 = inv * inv;\n"
        "    float inv4 = inv2 * inv2;\n"
        "    float inv5 = inv4 * inv;\n"
        "    return f0 + ((1.0 - f0) * inv5);\n"
        "}\n"
        // cos_theta is (n dot v)
        "vec3 fresnel_schlick_roughness(const float nv, const vec3 f0, const float roughness)\n"
        "{\n"
        "    float inv = 1.0 - nv;\n"
        "    float inv2 = inv * inv;\n"
        "    float inv4 = inv2 * inv2;\n"
        "    float inv5 = inv4 * inv;\n"
        "    return f0 + ((max(vec3(1.0 - roughness), f0) - f0) * inv5);\n"
        "}\n"
        "void main()\n"
        "{\n"
        //   material properties
        "    vec4 tmpv4 = texture(material_base_color, out_uv);\n"
        "    tmpv4.w *= material_alpha;\n"
        "    vec4 albedo = tmpv4;\n"
        "    if(albedo.w < material_alpha_cutoff) discard;\n"
        "	 tmpv4.xy = texture(material_metallic_roughness, out_uv).xy;\n"
        "    tmpv4.xy *= vec2(material_metallic_factor, material_roughness_factor);\n"
        "    float metallic = tmpv4.x;\n"
        "    float roughness = tmpv4.y;\n"
        //   TODO: in future maybe I will add ao in here
        //   input lighting data
        "    vec3 normal = mat3(out_tng, out_btg, out_nrm) * ((texture(material_normal, out_uv).xyz - 0.5) * 2.0 * material_normal_scale);\n"
        "    vec3 view = normalize(camera_position - out_pos);\n"
        "    vec3 reflection = reflect(-view, normal);\n"
        "    float normal_dot_view = max(dot(normal, view), 0.0);\n"
        //   calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
        //   of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
        //   TODO: in future I must bring builder fresnel factor 0 (the hard coded 0.4) from matrial uniform data
        "    vec3 f0 = mix(vec3(0.04), albedo.xyz, metallic);\n"
        //   reflectance equation
        "    vec3 lo = vec3(0.0);\n"
        //   computing point lights
        "    for (float i = 0.001; i < scene_lights_count.y; ++i)\n"
        "    {\n"
        "        int ii = int(i);\n"
        //       calculate per-light radiance
        "        vec3 light_vec = scene_point_lights_position_max_radius[ii].xyz - out_pos;\n"
        //       TODO: in future consider max and min radius
        "        float distance = length(light_vec);\n"
        "        float distance_inv = 1.0 / distance;\n"
        "        vec3 light_direction = light_vec * distance_inv;\n"
        "        float normal_dot_light = max(dot(normal, light_direction), 0.0);\n"
        "        vec3 half_vec = normalize(view + light_direction);\n"
        "        float attenuation = distance_inv * distance_inv;\n"
        "        vec3 radiance = scene_point_lights_color_min_radius[ii].xyz * attenuation;\n"
        //       Cook-Torrance BRDF
        "        float ndf = distribution_ggx(normal, half_vec, roughness);\n"
        "        float geo = geometry_smith(normal_dot_light, normal_dot_view, roughness);\n"
        "        vec3 frsn = fresnel_schlick(max(dot(half_vec, view), 0.0), f0);\n"
        "        vec3 nominator = ndf * geo * frsn;\n"
        //       0.001 to prevent divide by zero.
        "        float denominator = 4.0 * normal_dot_view * normal_dot_light + 0.001;\n"
        "        vec3 specular = nominator / denominator;\n"
        //       kS is equal to Fresnel
        "        vec3 ks = frsn;\n"
        //       for energy conservation, the diffuse and specular light can't
        //       be above 1.0 (unless the surface emits light); to preserve this
        //       relationship the diffuse component (kD) should equal 1.0 - kS.
        //       multiply kD by the inverse metalness such that only non-metals
        //       have diffuse lighting, or a linear blend if partly metal (pure metals
        //       have no diffuse light).
        "        vec3 kd = (vec3(1.0) - ks) * (1.0 - metallic);\n"
        //       scale light by NdotL
        //       add to outgoing radiance Lo
        //       note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
        "        lo += (kd * albedo.xyz / GXPI + specular) * radiance * normal_dot_light;\n"
        "    }\n"
        //   computing directional lights
        "    for (float i = 0.001; i < scene_lights_count.x; ++i)\n"
        "    {\n"
        "        int ii = int(i);\n"
        "        vec3 light_direction = -scene_directional_lights_direction[ii].xyz;\n"
        "        float normal_dot_light = max(dot(normal, light_direction), 0.0);\n"
        "        vec3 half_vec = normalize(view + light_direction);\n"
        "        vec3 radiance = scene_directional_lights_color[ii].xyz;\n"
        //       Cook-Torrance BRDF
        "        float ndf = distribution_ggx(normal, half_vec, roughness);\n"
        "        float geo = geometry_smith(normal_dot_light, normal_dot_view, roughness);\n"
        "        vec3 frsn = fresnel_schlick(max(dot(half_vec, view), 0.0), f0);\n"
        "        vec3 nominator = ndf * geo * frsn;\n"
        //       0.001 to prevent divide by zero.
        "        float denominator = 4.0 * normal_dot_view * normal_dot_light + 0.001;\n"
        "        vec3 specular = nominator / denominator;\n"
        //       kS is equal to Fresnel
        "        vec3 ks = frsn;\n"
        //       for energy conservation, the diffuse and specular light can't
        //       be above 1.0 (unless the surface emits light); to preserve this
        //       relationship the diffuse component (kD) should equal 1.0 - kS.
        //       multiply kD by the inverse metalness such that only non-metals
        //       have diffuse lighting, or a linear blend if partly metal (pure metals
        //       have no diffuse light).
        "        vec3 kd = (vec3(1.0) - ks) * (1.0 - metallic);\n"
        //       scale light by NdotL
        //       add to outgoing radiance Lo
        //       note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
        "        lo += (kd * albedo.xyz / GXPI + specular) * radiance * normal_dot_light;\n"
        "    }\n"
        "    bool is_in_directional_light = true;\n"
        //   TODO: I'm not sure about bias calculation, maybe I should calculate it by vertex normal
        "    float normal_dot_light = max(dot(normal, -light_direction), 0.0);\n"
        "    float shadow_bias = 0.001;\n"
        "    if(normal_dot_light > 0.0)\n"
        "    {\n"
        "        shadow_bias = clamp(sqrt((0.000025 / (normal_dot_light * normal_dot_light)) - 0.000025), 0.001, 0.02);\n"
        "    }\n"
        "    else\n"
        "    {\n"
        "        is_in_directional_light = false;"
        "    }\n"
        "    if(is_in_directional_light)\n"
        "    {\n"
        "        int effect_cascades_count_int = int(effect_cascades_count);\n"
        "        for(int i = 0; i < effect_cascades_count_int; ++i)\n"
        "        {\n"
        "            vec3 lightuv = out_light_poses[i];\n"
        "            if (lightuv.x > 0.0 && lightuv.x < 1.0 && lightuv.y > 0.0 && lightuv.y < 1.0)\n"
        "            {\n"
        //               TODO: it must become for each cascade shadow map
        //        "                float depth = texture(effect_shadow_map, lightuv, shadow_bias);\n"
        //"                float depth = texture(effect_shadow_map, lightuv.xy).x * 2.0 - 1.0;\n"
        "                float depth = texture(effect_shadow_map, lightuv.xy, 0.0).x;\n"
//        "                if(depth != 0.0) { frag_color = vec4(vec3(abs(depth)), 1.0); return; }\n"
        "                if(depth + shadow_bias <= lightuv.z)\n"
        //        "                if(depth == 0.0)\n"
        "                {\n"
        "                    is_in_directional_light = false;\n"
        "                }\n"
        "                break;\n"
        "            }\n"
        "        }\n"
        "    }\n"
        "    if(is_in_directional_light)\n"
        "    {\n"
        "        vec3 half_vec = normalize(view - light_direction);\n"
        "        vec3 radiance = light_color.xyz;\n"
        //       Cook-Torrance BRDF
        "        float ndf = distribution_ggx(normal, half_vec, roughness);\n"
        "        float geo = geometry_smith(normal_dot_light, normal_dot_view, roughness);\n"
        "        vec3 frsn = fresnel_schlick(max(dot(half_vec, view), 0.0), f0);\n"
        "        vec3 nominator = ndf * geo * frsn;\n"
        //       0.001 to prevent divide by zero.
        "        float denominator = 4.0 * normal_dot_view * normal_dot_light + 0.001;\n"
        "        vec3 specular = nominator / denominator;\n"
        //       kS is equal to Fresnel
        "        vec3 ks = frsn;\n"
        //       for energy conservation, the diffuse and specular light can't
        //       be above 1.0 (unless the surface emits light); to preserve this
        //       relationship the diffuse component (kD) should equal 1.0 - kS.
        //       multiply kD by the inverse metalness such that only non-metals
        //       have diffuse lighting, or a linear blend if partly metal (pure metals
        //       have no diffuse light).
        "        vec3 kd = (vec3(1.0) - ks) * (1.0 - metallic);\n"
        //       scale light by NdotL
        //       add to outgoing radiance Lo
        //       note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
        "        lo += (kd * albedo.xyz / GXPI + specular) * radiance * normal_dot_light;\n"
        "    }\n"
        //   ambient lighting (we now use IBL as the ambient term)
        "    vec3 frsn = fresnel_schlick_roughness(normal_dot_view, f0, roughness);\n"
        "    vec3 ks = frsn;\n"
        "    vec3 kd = (1.0 - ks) * (1.0 - metallic);\n"
        "    vec3 irradiance = texture(effect_diffuse_environment, normal).rgb;\n"
        "    vec3 diffuse = irradiance * albedo.xyz;\n"
        //   sample both the pre-filter map and the BRDF lut and combine them together as per
        //   the Split-Sum approximation to get the IBL specular part.
        "    float MAX_REFLECTION_LOD = 4.0;\n"
        "    vec3 prefiltered_color = texture(effect_specular_environment, reflection, roughness * MAX_REFLECTION_LOD).rgb;\n"
        "    vec2 brdf = texture(effect_brdflut, vec2(normal_dot_view, roughness)).rg;\n"
        "    vec3 specular = prefiltered_color * (frsn * brdf.x + brdf.y);\n"
        //   TODO: add ambient occlusion (* ao);
        "    vec3 ambient = kd * diffuse + specular;\n"
        "    tmpv4.xyz = ambient + lo;\n"
        //   HDR tonemapping
        "    tmpv4.xyz = tmpv4.xyz / (tmpv4.xyz + vec3(1.0));\n"
        //   gamma correct
        "    tmpv4.xyz = pow(tmpv4.xyz, vec3(1.0 / 2.2));\n"
        //   TODO don't forget gamma correction it can be part of scene uniform data
        "    frag_color = vec4(tmpv4.xyz, albedo.w);\n"
        "}";
    e->get_function_loader()->load([this, vertex_shader_code { move(vertex_shader_code.str()) }, fragment_shader_code { move(fragment_shader_code.str()) }] {
        set_vertex_shader(vertex_shader_code);
        set_fragment_shader(fragment_shader_code);
        link();
        GX_GLC3_SHADER_SET_TEXTURE_INDEX_STARTING
        GX_GLC3_SHADER_MATERIAL_GET_UNIFORM_LOCATIONS
        GX_GLC3_THIS_GET_UNIFORM_F(camera_position)
        GX_GLC3_THIS_GET_UNIFORM_F(camera_vp)
        // TODO
        //GX_GLC3_THIS_GET_UNIFORM_F(effect_ambient_occlusion)
        GX_GLC3_THIS_GET_UNIFORM_TEXTURE_F(effect_brdflut)
        GX_GLC3_THIS_GET_UNIFORM_F(effect_cascades_count)
        GX_GLC3_THIS_GET_UNIFORM_TEXTURE_F(effect_diffuse_environment)
        GX_GLC3_THIS_GET_UNIFORM_TEXTURE_F(effect_shadow_map)
        GX_GLC3_THIS_GET_UNIFORM_TEXTURE_F(effect_specular_environment)
        GX_GLC3_THIS_GET_UNIFORM_F(effect_view_projection_biases)
        GX_GLC3_THIS_GET_UNIFORM_F(light_color)
        GX_GLC3_THIS_GET_UNIFORM_F(light_direction)
        GX_GLC3_THIS_GET_UNIFORM_F(model_m)
        // GX_GLES2_THIS_GET_UNIFORM_F(scene_ambient_light)
        GX_GLC3_THIS_GET_UNIFORM_F(scene_directional_lights_color)
        GX_GLC3_THIS_GET_UNIFORM_F(scene_directional_lights_direction)
        GX_GLC3_THIS_GET_UNIFORM_F(scene_lights_count)
        GX_GLC3_THIS_GET_UNIFORM_F(scene_point_lights_color_min_radius)
        GX_GLC3_THIS_GET_UNIFORM_F(scene_point_lights_position_max_radius)
        // GX_GLES2_THIS_GET_UNIFORM_F(scene_ssao_config)
    });
}

gearoenix::glc3::shader::ForwardPbrDirectionalShadow::~ForwardPbrDirectionalShadow() noexcept
{
}

void gearoenix::glc3::shader::ForwardPbrDirectionalShadow::bind() const noexcept
{
    Shader::bind();
    GX_GLC3_SHADER_MATERIAL_SET_TEXTURE_INDEX_UNIFORM
    GX_GLC3_SHADER_SET_TEXTURE_INDEX_UNIFORM(effect_ambient_occlusion)
    GX_GLC3_SHADER_SET_TEXTURE_INDEX_UNIFORM(effect_brdflut)
    GX_GLC3_SHADER_SET_TEXTURE_INDEX_UNIFORM(effect_diffuse_environment)
    GX_GLC3_SHADER_SET_TEXTURE_INDEX_UNIFORM(effect_shadow_map)
    GX_GLC3_SHADER_SET_TEXTURE_INDEX_UNIFORM(effect_specular_environment)
}
#endif
