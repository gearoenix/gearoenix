#include "gx-gl-shd-deferred-pbr-transparent.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED

namespace {
constexpr auto vertex_shader_src = "\
#version 300 es\n\
\n\
#define gx_pi 3.141592653589793238\n\
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

constexpr auto fragment_shader_src = "\
#version 300 es\n\
\n\
#define gx_pi 3.141592653589793238\n\
\n\
precision highp float;\n\
precision highp int;\n\
precision highp sampler2D;\n\
precision highp samplerCube;\n\
\n\
uniform vec4 screen_uv_move_reserved;\n\
\n\
uniform sampler2D albedo_metallic;\n\
uniform sampler2D position_depth;\n\
uniform sampler2D normal_ao;\n\
uniform sampler2D emission_roughness;\n\
uniform sampler2D ssao_resolved;\n\
\n\
in vec2 out_uv;\n\
\n\
out vec4 frag_colour;\n\
\n\
void main() {\n\
\n\
    vec4 alb_mtl = texture(albedo_metallic, out_uv);\n\
    vec4 pos_dpt = texture(position_depth, out_uv);\n\
    vec4 nrm_ao = texture(normal_ao, out_uv);\n\
    vec4 ems_rgh = texture(emission_roughness, out_uv);\n\
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
    frag_colour = vec4(vec3(ssao_value), 1.0) +\n\
        (alb_mtl + pos_dpt + ems_rgh) * 0.00001;\n\
}\n";
}

gearoenix::gl::shader::DeferredPbrTransparent::DeferredPbrTransparent()
{
    set_vertex_shader(vertex_shader_src);
    set_fragment_shader(fragment_shader_src);
    link();
    GX_GL_SHADER_SET_TEXTURE_INDEX_STARTING;
    GX_GL_THIS_GET_UNIFORM(screen_uv_move_reserved);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(albedo_metallic);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(position_depth);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(normal_ao);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(emission_roughness);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(ssao_resolved);
}

gearoenix::gl::shader::DeferredPbrTransparent::~DeferredPbrTransparent() = default;

void gearoenix::gl::shader::DeferredPbrTransparent::bind(uint& current_shader) const
{
    if (shader_program == current_shader)
        return;
    Shader::bind(current_shader);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(albedo_metallic);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(position_depth);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(normal_ao);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(emission_roughness);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(ssao_resolved);
}

#endif