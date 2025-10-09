#include "gx-gl-shd-deferred-pbr.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED

namespace {
constexpr auto vertex_shader_body = R"SHADER(
layout(location = 0) in vec2 position;

out vec2 out_uv;

void main() {
    gl_Position = vec4(position, 0.0, 1.0);
    out_uv = position* 0.5 + 0.5;
}
)SHADER";

constexpr auto fragment_shader_body = R"SHADER(
uniform vec4 screen_uv_move_reserved;
uniform vec3 camera_position;

uniform sampler2D albedo_metallic;
uniform sampler2D position_depth;
uniform sampler2D normal_ao;
uniform sampler2D emission_roughness;
uniform sampler2D ssao_resolved;
uniform sampler2D irradiance;
uniform sampler2D radiance;
uniform sampler2D brdflut;

in vec2 out_uv;

out vec4 frag_colour;

vec3 fresnel_schlick_roughness(const float nv, const vec3 f0, const float roughness)
{
    float inv = 1.0 - nv;
    float inv2 = inv * inv;
    float inv4 = inv2 * inv2;
    float inv5 = inv4 * inv;
    return f0 + ((max(vec3(1.0 - roughness), f0) - f0) * inv5);
}

void main() {

    vec4 pos_dpt = texture(position_depth, out_uv);
    if(pos_dpt.w >= 1.0) discard;
    vec4 alb_mtl = texture(albedo_metallic, out_uv);
    vec4 nrm_ao = texture(normal_ao, out_uv);
    vec4 ems_rgh = texture(emission_roughness, out_uv);
    vec3 irr = texture(irradiance, out_uv).xyz;
    vec3 rad = texture(radiance, out_uv).xyz;

    float ssao_main_pixel_value = texture(ssao_resolved, out_uv).x;
    float ssao_value = ssao_main_pixel_value;
    ssao_value += texture(ssao_resolved, out_uv + screen_uv_move_reserved.xy).x;
    ssao_value += texture(ssao_resolved, out_uv - screen_uv_move_reserved.xy).x;
    ssao_value += texture(ssao_resolved, out_uv + vec2(screen_uv_move_reserved.x, 0.0)).x;
    ssao_value += texture(ssao_resolved, out_uv - vec2(screen_uv_move_reserved.x, 0.0)).x;
    ssao_value += texture(ssao_resolved, out_uv + vec2(0.0, screen_uv_move_reserved.y)).x;
    ssao_value += texture(ssao_resolved, out_uv - vec2(0.0, screen_uv_move_reserved.y)).x;
    ssao_value += texture(ssao_resolved, out_uv + vec2(screen_uv_move_reserved.x, -screen_uv_move_reserved.y)).x;
    ssao_value += texture(ssao_resolved, out_uv + vec2(-screen_uv_move_reserved.x, screen_uv_move_reserved.y)).x;
    ssao_value = 1.0 - (step(ssao_main_pixel_value, 0.999) * (1.0 - ssao_value * 0.1111111111111111111111111));
    ssao_value *= nrm_ao.w;

    vec3 f0 = mix(vec3(0.04), alb_mtl.xyz, alb_mtl.w);
    vec3 f0_inv = vec3(1.0) - f0;
    vec3 eye = normalize(pos_dpt.xyz - camera_position);
    vec3 reflection = normalize(reflect(eye, nrm_ao.xyz));
    float normal_dot_view = -dot(nrm_ao.xyz, eye);
    vec3 fresnel = fresnel_schlick_roughness(normal_dot_view, f0, ems_rgh.w);
    vec3 kd = (vec3(1.0) - fresnel) * (1.0 - alb_mtl.w);
    vec3 ambient = irr * alb_mtl.xyz * kd;
    vec2 brdf = texture(brdflut, vec2(normal_dot_view, ems_rgh.w)).rg;
    ambient += rad * ((fresnel * brdf.x) + brdf.y);
    ambient *= ssao_value;
    frag_colour = vec4(ambient + ems_rgh.xyz, 1.0);
}
)SHADER";
}

gearoenix::gl::shader::DeferredPbr::DeferredPbr()
{
    set_vertex_shader(get_common_shader_starter() + vertex_shader_body);
    set_fragment_shader(get_common_shader_starter() + fragment_shader_body);
    link();
    GX_GL_SHADER_SET_TEXTURE_INDEX_STARTING;
    GX_GL_THIS_GET_UNIFORM(screen_uv_move_reserved);
    GX_GL_THIS_GET_UNIFORM(camera_position);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(albedo_metallic);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(position_depth);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(normal_ao);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(emission_roughness);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(ssao_resolved);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(irradiance);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(radiance);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(brdflut);
}

gearoenix::gl::shader::DeferredPbr::~DeferredPbr() = default;

void gearoenix::gl::shader::DeferredPbr::bind(uint& current_shader) const
{
    if (shader_program == current_shader)
        return;
    Shader::bind(current_shader);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(albedo_metallic);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(position_depth);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(normal_ao);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(emission_roughness);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(ssao_resolved);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(irradiance);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(radiance);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(brdflut);
}

#endif