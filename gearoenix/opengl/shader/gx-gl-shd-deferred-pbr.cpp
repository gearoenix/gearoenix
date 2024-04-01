#include "gx-gl-shd-deferred-pbr.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED

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
layout(location = 0) in vec2 position;\n\
\n\
out vec2 out_uv;\n\
\n\
void main() {\n\
    gl_Position = vec4(position, 0.0, 1.0);\n\
    out_uv = position* 0.5 + 0.5;\n\
}\n";

static constexpr const char* const fragment_shader_src = "\
#version 300 es\n\
\n\
#define GX_PI 3.141592653589793238\n\
\n\
precision highp float;\n\
precision highp int;\n\
precision highp sampler2D;\n\
precision highp samplerCube;\n\
\n\
uniform vec4 screen_uv_move_reserved;\n\
uniform vec3 camera_position;\n\
\n\
uniform sampler2D albedo_metallic;\n\
uniform sampler2D position_depth;\n\
uniform sampler2D normal_ao;\n\
uniform sampler2D emission_roughness;\n\
uniform sampler2D ssao_resolved;\n\
uniform sampler2D irradiance;\n\
uniform sampler2D radiance;\n\
uniform sampler2D brdflut;\n\
\n\
in vec2 out_uv;\n\
\n\
out vec4 frag_colour;\n\
\n\
vec3 fresnel_schlick_roughness(const float nv, const vec3 f0, const float roughness)\n\
{\n\
    float inv = 1.0 - nv;\n\
    float inv2 = inv * inv;\n\
    float inv4 = inv2 * inv2;\n\
    float inv5 = inv4 * inv;\n\
    return f0 + ((max(vec3(1.0 - roughness), f0) - f0) * inv5);\n\
}\n\
\n\
void main() {\n\
\n\
    vec4 pos_dpt = texture(position_depth, out_uv);\n\
    if(pos_dpt.w >= 1.0) discard;\n\
    vec4 alb_mtl = texture(albedo_metallic, out_uv);\n\
    vec4 nrm_ao = texture(normal_ao, out_uv);\n\
    vec4 ems_rgh = texture(emission_roughness, out_uv);\n\
    vec3 irr = texture(irradiance, out_uv).xyz;\n\
    vec3 rad = texture(radiance, out_uv).xyz;\n\
\n\
    float ssao_main_pixel_value = texture(ssao_resolved, out_uv).x;\n\
    float ssao_value = ssao_main_pixel_value;\n\
    ssao_value += texture(ssao_resolved, out_uv + screen_uv_move_reserved.xy).x;\n\
    ssao_value += texture(ssao_resolved, out_uv - screen_uv_move_reserved.xy).x;\n\
    ssao_value += texture(ssao_resolved, out_uv + vec2(screen_uv_move_reserved.x, 0.0)).x;\n\
    ssao_value += texture(ssao_resolved, out_uv - vec2(screen_uv_move_reserved.x, 0.0)).x;\n\
    ssao_value += texture(ssao_resolved, out_uv + vec2(0.0, screen_uv_move_reserved.y)).x;\n\
    ssao_value += texture(ssao_resolved, out_uv - vec2(0.0, screen_uv_move_reserved.y)).x;\n\
    ssao_value += texture(ssao_resolved, out_uv + vec2(screen_uv_move_reserved.x, -screen_uv_move_reserved.y)).x;\n\
    ssao_value += texture(ssao_resolved, out_uv + vec2(-screen_uv_move_reserved.x, screen_uv_move_reserved.y)).x;\n\
    ssao_value = 1.0 - (step(ssao_main_pixel_value, 0.999) * (1.0 - ssao_value * 0.1111111111111111111111111));\n\
    ssao_value *= nrm_ao.w;\n\
\n\
    vec3 f0 = mix(vec3(0.04), alb_mtl.xyz, alb_mtl.w);\n\
    vec3 f0_inv = vec3(1.0) - f0;\n\
    vec3 eye = normalize(pos_dpt.xyz - camera_position);\n\
    vec3 reflection = normalize(reflect(eye, nrm_ao.xyz));\n\
    float normal_dot_view = -dot(nrm_ao.xyz, eye);\n\
    vec3 fresnel = fresnel_schlick_roughness(normal_dot_view, f0, ems_rgh.w);\n\
    vec3 kd = (vec3(1.0) - fresnel) * (1.0 - alb_mtl.w);\n\
    vec3 ambient = irr * alb_mtl.xyz * kd;\n\
    vec2 brdf = texture(brdflut, vec2(normal_dot_view, ems_rgh.w)).rg;\n\
    ambient += rad * ((fresnel * brdf.x) + brdf.y);\n\
    ambient *= ssao_value;\n\
    frag_colour = vec4(ambient + ems_rgh.xyz, 1.0);\n\
}\n";

gearoenix::gl::shader::DeferredPbr::DeferredPbr(Engine& e)
    : Shader(e)
{
    set_vertex_shader(vertex_shader_src);
    set_fragment_shader(fragment_shader_src);
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