#include "gx-gl-shd-forward-pbr.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../core/macro/gx-cr-mcr-stringifier.hpp"
#include "../gx-gl-engine.hpp"
#include <sstream>
#include <string>

static constexpr const char* const vertex_shader_src = "\
#version 300 es\n\
\n\
#define GX_PI 3.141592653589793238\n\
\n\
precision highp float;\n\
precision highp int;\n\
precision highp sampler2D;\n\
precision highp samplerCube;\n\
\n\
layout(location = " GX_STRINGIFY(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_POSITION) ") in vec3 position;\n\
layout(location = " GX_STRINGIFY(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_NORMAL) ") in vec3 normal;\n\
layout(location = " GX_STRINGIFY(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_TANGENT) ") in vec4 tangent;\n\
layout(location = " GX_STRINGIFY(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_UV) ") in vec2 uv;\n\
\n\
uniform mat4 m;\n\
uniform mat4 inv_m;\n\
uniform mat4 vp;\n\
\n\
out vec3 out_pos;\n\
out vec3 out_nrm;\n\
out vec3 out_tng;\n\
out vec3 out_btg;\n\
out vec2 out_uv;\n\
\n\
void main() {\n\
    out_uv = uv;\n\
    vec4 pos = m * vec4(position, 1.0);\n\
    out_pos = pos.xyz;\n\
    out_nrm = (inv_m * vec4(normal, 0.0)).xyz;\n\
    out_tng = (inv_m * vec4(tangent.xyz, 0.0)).xyz;\n\
    out_btg = cross(out_nrm, out_tng) * tangent.w;\n\
    gl_Position = vp * pos;\n\
}\n";

gearoenix::gl::shader::ForwardPbr::ForwardPbr(
    Engine& e,
    const std::size_t shadow_casters_directional_lights_count) noexcept
    : Shader(e)
    , shadow_caster_directional_light_shadow_map_indices(shadow_casters_directional_lights_count)
{
    std::stringstream ss;
    // Vertex Shader -----------------------------------------------------------------------------------
    set_vertex_shader(vertex_shader_src);
    // Fragment Shader ---------------------------------------------------------------------------------
    ss.clear();
    ss << "#version 300 es\n";
    ss << "\n";
    ss << "#define GX_PI 3.141592653589793238\n";
    ss << "\n";
    ss << "precision highp float;\n";
    ss << "precision highp int;\n";
    ss << "precision highp sampler2D;\n";
    ss << "precision highp samplerCube;\n";
    ss << "\n";
    ss << "uniform vec4 camera_position_reserved;\n";
    ss << "uniform vec4 albedo_factor;\n";
    ss << "uniform vec4 normal_metallic_factor;\n";
    ss << "uniform vec4 emission_roughness_factor;\n";
    ss << "uniform vec4 alpha_cutoff_occlusion_strength_radiance_lod_coefficient_reserved;\n";
    ss << "\n";
    if (shadow_casters_directional_lights_count > 0) {
        ss << "uniform vec3 shadow_caster_directional_light_direction[" << shadow_casters_directional_lights_count << "];\n";
        ss << "uniform vec3 shadow_caster_directional_light_colour[" << shadow_casters_directional_lights_count << "];\n";
        ss << "uniform mat4 shadow_caster_directional_light_normalised_vp[" << shadow_casters_directional_lights_count << "];\n";
        ss << "uniform sampler2D shadow_caster_directional_light_shadow_map[" << shadow_casters_directional_lights_count << "];\n";
    }
    ss << "\n";
    ss << "uniform sampler2D albedo;\n";
    ss << "uniform sampler2D normal;\n";
    ss << "uniform sampler2D emission;\n";
    ss << "uniform sampler2D metallic_roughness;\n";
    ss << "uniform sampler2D occlusion;\n";
    ss << "uniform sampler2D brdflut;\n";
    ss << "\n";
    ss << "uniform samplerCube irradiance;\n";
    ss << "uniform samplerCube radiance;\n";
    ss << "\n";
    ss << "in vec3 out_pos;\n";
    ss << "in vec3 out_nrm;\n";
    ss << "in vec3 out_tng;\n";
    ss << "in vec3 out_btg;\n";
    ss << "in vec2 out_uv;\n";
    ss << "\n";
    ss << "layout(location = " << GEAROENIX_GL_BGCAAS_FRAMEBUFFER_ATTACHMENT_INDEX_LOW << ") out vec4 frag_out_low;\n";
    ss << "layout(location = " << GEAROENIX_GL_BGCAAS_FRAMEBUFFER_ATTACHMENT_INDEX_HIGH << ") out vec4 frag_out_high;\n";
    ss << "\n";
    ss << "vec3 fresnel_schlick_roughness(const float nv, const vec3 f0, const float roughness) {\n";
    ss << "    float inv = 1.0 - nv;\n";
    ss << "    float inv2 = inv * inv;\n";
    ss << "    float inv4 = inv2 * inv2;\n";
    ss << "    float inv5 = inv4 * inv;\n";
    ss << "    return f0 + ((max(vec3(1.0 - roughness), f0) - f0) * inv5);\n";
    ss << "}\n";
    ss << "\n";
    ss << "float distribution_ggx(const vec3 normal, const vec3 halfway, const float roughness_p2, const float roughness_p2_minus_1) {\n";
    ss << "    float denom = max(dot(normal, halfway), 0.0);\n";
    ss << "    denom *= denom;\n";
    ss << "    denom *= roughness_p2_minus_1;\n";
    ss << "    denom += 1.0;\n";
    ss << "    denom *= denom;\n";
    ss << "    denom *= GX_PI;\n";
    ss << "    return roughness_p2 / denom;\n";
    ss << "}\n";
    ss << "\n";
    ss << "float geometry_schlick_ggx(const float nd, const float geometry_schlick_k, const float geometry_schlick_k_inv) {\n";
    ss << "    return 1.0 / (nd * geometry_schlick_k_inv + geometry_schlick_k);\n";
    ss << "}\n";
    ss << "\n";
    ss << "float geometry_smith(const float normal_dot_light, const float normal_dot_view, const float geometry_schlick_k, const float geometry_schlick_k_inv) {\n";
    ss << "    float ggx2 = geometry_schlick_ggx(normal_dot_view, geometry_schlick_k, geometry_schlick_k_inv);\n";
    ss << "    float ggx1 = geometry_schlick_ggx(normal_dot_light, geometry_schlick_k, geometry_schlick_k_inv);\n";
    ss << "    return ggx1 * ggx2;\n";
    ss << "}\n";
    ss << "\n";
    ss << "vec3 fresnel_schlick(const float nv, const vec3 f0, const vec3 f0_inv) {\n";
    ss << "    float inv = 1.0 - nv;\n";
    ss << "    float inv2 = inv * inv;\n";
    ss << "    float inv4 = inv2 * inv2;\n";
    ss << "    float inv5 = inv4 * inv;\n";
    ss << "    return f0 + (f0_inv * inv5);\n";
    ss << "}\n";
    ss << "\n";
    ss << "vec3 compute_light(\n";
    ss << "        const vec3 light_direction,\n";
    ss << "        const vec3 light_color,\n";
    ss << "        const vec3 view,\n";
    ss << "        const vec3 normal,\n";
    ss << "        const float normal_dot_view,\n";
    ss << "        const float roughness_p2,\n";
    ss << "        const float roughness_p2_minus_1,\n";
    ss << "        const float geometry_schlick_k,\n";
    ss << "        const float geometry_schlick_k_inv,\n";
    ss << "        const vec3 f0,\n";
    ss << "        const vec3 f0_inv,\n";
    ss << "        const float one_minus_metallic,\n";
    ss << "        const vec3 albedo) {\n";
    ss << "    float normal_dot_light = max(dot(normal, light_direction), 0.0001);\n";
    ss << "    vec3 half_vec = normalize(view + light_direction);\n";
    ss << "    float ndf = distribution_ggx(normal, half_vec, roughness_p2, roughness_p2_minus_1);\n";
    ss << "    float geo = geometry_smith(normal_dot_light, normal_dot_view, geometry_schlick_k, geometry_schlick_k_inv);\n";
    ss << "    vec3 fresnel = fresnel_schlick(dot(half_vec, view), f0, f0_inv);\n";
    ss << "    vec3 specular = ndf * geo * fresnel * 0.25;\n";
    ss << "    vec3 ks = fresnel;\n";
    ss << "    vec3 kd = (vec3(1.0) - ks) * one_minus_metallic;\n";
    ss << "    return (kd * albedo * ( 1.0 / GX_PI) + specular) * light_color * normal_dot_light;\n";
    ss << "}\n";
    ss << "\n";
    ss << "void main() {\n";
    ss << "\n";
    ss << "    vec4 alb = texture(albedo, out_uv) * albedo_factor;\n";
    ss << "    if(alb.w <= alpha_cutoff_occlusion_strength_radiance_lod_coefficient_reserved.x) discard;\n";
    ss << "    vec2 mtr = texture(metallic_roughness, out_uv).xy * vec2(normal_metallic_factor.w, emission_roughness_factor.w);\n";
    ss << "    float ao = texture(occlusion, out_uv).x * alpha_cutoff_occlusion_strength_radiance_lod_coefficient_reserved.y;\n";
    ss << "    vec3 nrm = normalize(mat3(out_tng, out_btg, out_nrm) * ((texture(normal, out_uv).xyz * 2.0 - 1.0) * normal_metallic_factor.xyz));\n";
    ss << "    vec3 ems = texture(emission, out_uv).xyz * emission_roughness_factor.xyz;\n";
    ss << "    vec3 f0 = mix(vec3(0.04), alb.xyz, mtr.x);\n";
    ss << "    vec3 f0_inv = vec3(1.0) - f0;\n";
    ss << "    vec3 eye = normalize(out_pos - camera_position_reserved.xyz);\n";
    ss << "    vec3 reflection = reflect(eye, nrm);\n";
    ss << "    float normal_dot_view = max(-dot(nrm, eye), 0.0001);\n";
    ss << "    vec3 fresnel = fresnel_schlick_roughness(normal_dot_view, f0, mtr.y);\n";
    ss << "    vec3 kd = (vec3(1.0) - fresnel) * (1.0 - mtr.y);\n";
    ss << "    vec3 lumination = vec3(0.0001);\n";
    ss << "    float roughness_p2 = mtr.y * mtr.y;\n";
    ss << "    float roughness_p2_minus_1 = roughness_p2 - 1.0;\n";
    ss << "    float geometry_schlick_k = mtr.y + 1.0;\n";
    ss << "    geometry_schlick_k *= geometry_schlick_k;\n";
    ss << "    geometry_schlick_k *= 0.125;\n";
    ss << "    float geometry_schlick_k_inv = 1.0 - geometry_schlick_k;\n";
    ss << "    float one_minus_metallic = 1.0 - mtr.x;\n";
    ss << "\n";
    if (shadow_casters_directional_lights_count > 0) {
        ss << "    for (int dir_i = 0; dir_i < " << shadow_casters_directional_lights_count << "; ++dir_i) {\n";
        ss << "        float normal_dot_light = max(dot(nrm, shadow_caster_directional_light_direction[dir_i]), 0.00001);\n";
        ss << "        float shadow_bias = clamp(sqrt((0.000025 / (normal_dot_light * normal_dot_light)) - 0.000025), 0.001, 0.02);\n";
        ss << "        vec4 light_uv_depth = shadow_caster_directional_light_normalised_vp[dir_i] * vec4(out_pos, 1.0);\n";
        ss << "        light_uv_depth.xyz /= light_uv_depth.w;\n";
        ss << "        light_uv_depth.xyz *= 0.5;\n";
        ss << "        light_uv_depth.xyz += 0.5;\n";
        ss << "        vec2 uv_bounds = step(vec2(0.0), light_uv_depth.xy) * step(light_uv_depth.xy, vec2(1.0));\n";
        ss << "        float shadow_w = uv_bounds.x * uv_bounds.y;\n";
        ss << "        float depth = texture(shadow_caster_directional_light_shadow_map[dir_i], light_uv_depth.xy, 0.0).x;\n";
        ss << "        float light_w = 1.0 - (step(depth + shadow_bias, light_uv_depth.z) * shadow_w);\n";
        ss << "        lumination += light_w * compute_light(\n";
        ss << "            shadow_caster_directional_light_direction[dir_i],\n";
        ss << "            shadow_caster_directional_light_colour[dir_i],\n";
        ss << "            -eye,\n";
        ss << "            nrm,\n";
        ss << "            normal_dot_view,\n";
        ss << "            roughness_p2,\n";
        ss << "            roughness_p2_minus_1,\n";
        ss << "            geometry_schlick_k,\n";
        ss << "            geometry_schlick_k_inv,\n";
        ss << "            f0,\n";
        ss << "            f0_inv,\n";
        ss << "            one_minus_metallic,\n";
        ss << "            alb.xyz);\n";
        ss << "    }\n";
    }
    ss << "\n";
    ss << "    vec3 irr = texture(irradiance, nrm).xyz;\n";
    ss << "    vec3 rad = textureLod(radiance, reflection, mtr.y * alpha_cutoff_occlusion_strength_radiance_lod_coefficient_reserved.z).xyz;\n";
    ss << "    vec3 ambient = irr * alb.xyz * kd;\n";
    ss << "    vec2 brdf = texture(brdflut, vec2(normal_dot_view, mtr.y)).rg;\n";
    ss << "    ambient += rad * ((fresnel * brdf.x) + brdf.y);\n";
    ss << "    ambient *= ao;\n";
    ss << "    vec3 frag_colour = ambient + ems + lumination;\n";
    ss << "    frag_out_high = vec4(frag_colour * step(1.0, dot(frag_colour, vec3(0.2126, 0.7152, 0.0722))), 1.0);\n";
    ss << "    frag_out_low = vec4(frag_colour - frag_out_high.xyz, 1.0);\n";
    ss << "}\n";
    set_fragment_shader(ss.str());

    link();

    GX_GL_SHADER_SET_TEXTURE_INDEX_STARTING;
    GX_GL_THIS_GET_UNIFORM(vp);
    GX_GL_THIS_GET_UNIFORM(camera_position_reserved);
    GX_GL_THIS_GET_UNIFORM(m);
    GX_GL_THIS_GET_UNIFORM(inv_m);
    GX_GL_THIS_GET_UNIFORM(albedo_factor);
    GX_GL_THIS_GET_UNIFORM(normal_metallic_factor);
    GX_GL_THIS_GET_UNIFORM(emission_roughness_factor);
    GX_GL_THIS_GET_UNIFORM(alpha_cutoff_occlusion_strength_radiance_lod_coefficient_reserved);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(albedo);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(normal);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(emission);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(metallic_roughness);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(occlusion);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(irradiance);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(radiance);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(brdflut);

    if (shadow_casters_directional_lights_count > 0) {
        GX_GL_THIS_GET_UNIFORM(shadow_caster_directional_light_direction);
        GX_GL_THIS_GET_UNIFORM(shadow_caster_directional_light_colour);
        GX_GL_THIS_GET_UNIFORM(shadow_caster_directional_light_normalised_vp);
        GX_GL_THIS_GET_UNIFORM(shadow_caster_directional_light_shadow_map);
        GX_GL_THIS_GET_UNIFORM_TEXTURE_ARRAY(shadow_caster_directional_light_shadow_map);
    }
}

gearoenix::gl::shader::ForwardPbr::~ForwardPbr() noexcept = default;

void gearoenix::gl::shader::ForwardPbr::bind() const noexcept
{
    Shader::bind();
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(albedo);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(normal);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(emission);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(metallic_roughness);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(occlusion);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(irradiance);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(radiance);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(brdflut);
    GX_GL_SHADER_SET_TEXTURE_INDEX_DYNAMIC_ARRAY_UNIFORM(shadow_caster_directional_light_shadow_map);
}

void gearoenix::gl::shader::ForwardPbr::set_shadow_caster_directional_light_normalised_vp_data(const void* const data) noexcept
{
    glUniformMatrix4fv(
        shadow_caster_directional_light_normalised_vp,
        static_cast<sizei>(shadow_caster_directional_light_shadow_map_indices.size()),
        GL_FALSE,
        reinterpret_cast<const float*>(data));
}

void gearoenix::gl::shader::ForwardPbr::set_shadow_caster_directional_light_direction_data(const void* const data) noexcept
{
    glUniform3fv(
        shadow_caster_directional_light_direction,
        static_cast<sizei>(shadow_caster_directional_light_shadow_map_indices.size()),
        reinterpret_cast<const float*>(data));
}

void gearoenix::gl::shader::ForwardPbr::set_shadow_caster_directional_light_colour_data(const void* const data) noexcept
{
    glUniform3fv(
        shadow_caster_directional_light_colour,
        static_cast<sizei>(shadow_caster_directional_light_shadow_map_indices.size()),
        reinterpret_cast<const float*>(data));
}

const gearoenix::gl::sint* gearoenix::gl::shader::ForwardPbr::get_shadow_caster_directional_light_shadow_map_indices() const noexcept
{
    if (shadow_caster_directional_light_shadow_map_indices.size() == 0)
        return nullptr;
    return shadow_caster_directional_light_shadow_map_indices.data();
}

#endif