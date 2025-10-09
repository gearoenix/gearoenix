#include "gx-gl-shd-deferred-pbr-transparent.hpp"
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

uniform sampler2D albedo_metallic;
uniform sampler2D position_depth;
uniform sampler2D normal_ao;
uniform sampler2D emission_roughness;
uniform sampler2D ssao_resolved;

in vec2 out_uv;

out vec4 frag_colour;

void main() {

    vec4 alb_mtl = texture(albedo_metallic, out_uv);
    vec4 pos_dpt = texture(position_depth, out_uv);
    vec4 nrm_ao = texture(normal_ao, out_uv);
    vec4 ems_rgh = texture(emission_roughness, out_uv);

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

    frag_colour = vec4(vec3(ssao_value), 1.0) +
        (alb_mtl + pos_dpt + ems_rgh) * 0.00001;
}
)SHADER";
}

gearoenix::gl::shader::DeferredPbrTransparent::DeferredPbrTransparent()
{
    set_vertex_shader(get_common_shader_starter() + vertex_shader_body);
    set_fragment_shader(get_common_shader_starter() + fragment_shader_body);
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