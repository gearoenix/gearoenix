#include "gx-gl-shd-ssao-resolve.hpp"
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
    out_uv = position * 0.5 + 0.5;\n\
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
const float ssao_samples_weight = 1.0 / 86.0;\n\
const int ssao_samples_count = 86;\n\
const vec3 ssao_samples[ssao_samples_count] = vec3[ssao_samples_count](\n\
 vec3(-0.5773502691896258, 0.5773502691896258, 0.5773502691896258),\n\
 vec3(-0.30290544652768625, 0.6738873386790492, 0.6738873386790492),\n\
 vec3(-0.6738873386790492, 0.30290544652768625, 0.6738873386790492),\n\
 vec3(-0.3676719239173477, 0.36139654819928557, 0.8568604853256786),\n\
 vec3(0.0, 0.7071067811865476, 0.7071067811865476),\n\
 vec3(0.30290544652768625, 0.6738873386790492, 0.6738873386790492),\n\
 vec3(0.0, 0.3826834323650898, 0.9238795325112867),\n\
 vec3(0.3676719239173477, 0.36139654819928557, 0.8568604853256786),\n\
 vec3(-0.7071067811865476, 0.0, 0.7071067811865476),\n\
 vec3(-0.3826834323650898, 0.0, 0.9238795325112867),\n\
 vec3(-0.6738873386790492, -0.30290544652768625, 0.6738873386790492),\n\
 vec3(-0.3676719239173477, -0.36139654819928557, 0.8568604853256786),\n\
 vec3(0.0, 0.0, 1.0),\n\
 vec3(0.3826834323650898, 0.0, 0.9238795325112867),\n\
 vec3(0.0, -0.3826834323650898, 0.9238795325112867),\n\
 vec3(0.3676719239173477, -0.36139654819928557, 0.8568604853256786),\n\
 vec3(-0.5773502691896258, 0.5773502691896258, -0.5773502691896258),\n\
 vec3(-0.30290544652768625, 0.6738873386790492, -0.6738873386790492),\n\
 vec3(-0.6738873386790492, 0.30290544652768625, -0.6738873386790492),\n\
 vec3(-0.3676719239173477, 0.36139654819928557, -0.8568604853256786),\n\
 vec3(0.0, 0.7071067811865476, -0.7071067811865476),\n\
 vec3(0.30290544652768625, 0.6738873386790492, -0.6738873386790492),\n\
 vec3(0.0, 0.3826834323650898, -0.9238795325112867),\n\
 vec3(0.3676719239173477, 0.36139654819928557, -0.8568604853256786),\n\
 vec3(-0.7071067811865476, 0.0, -0.7071067811865476),\n\
 vec3(-0.3826834323650898, 0.0, -0.9238795325112867),\n\
 vec3(-0.6738873386790492, -0.30290544652768625, -0.6738873386790492),\n\
 vec3(-0.3676719239173477, -0.36139654819928557, -0.8568604853256786),\n\
 vec3(0.0, 0.0, -1.0),\n\
 vec3(0.3826834323650898, 0.0, -0.9238795325112867),\n\
 vec3(0.0, -0.3826834323650898, -0.9238795325112867),\n\
 vec3(0.3676719239173477, -0.36139654819928557, -0.8568604853256786),\n\
 vec3(0.5773502691896258, -0.5773502691896258, 0.5773502691896258),\n\
 vec3(0.6738873386790492, -0.30290544652768625, 0.6738873386790492),\n\
 vec3(0.6738873386790492, -0.6738873386790492, 0.30290544652768625),\n\
 vec3(0.8568604853256786, -0.3676719239173477, 0.36139654819928557),\n\
 vec3(0.7071067811865476, 0.0, 0.7071067811865476),\n\
 vec3(0.6738873386790492, 0.30290544652768625, 0.6738873386790492),\n\
 vec3(0.9238795325112867, 0.0, 0.3826834323650898),\n\
 vec3(0.8568604853256786, 0.3676719239173477, 0.36139654819928557),\n\
 vec3(0.7071067811865476, -0.7071067811865476, 0.0),\n\
 vec3(0.9238795325112867, -0.3826834323650898, 0.0),\n\
 vec3(0.6738873386790492, -0.6738873386790492, -0.30290544652768625),\n\
 vec3(0.8568604853256786, -0.3676719239173477, -0.36139654819928557),\n\
 vec3(1.0, 0.0, 0.0),\n\
 vec3(0.9238795325112867, 0.3826834323650898, 0.0),\n\
 vec3(0.9238795325112867, 0.0, -0.3826834323650898),\n\
 vec3(0.8568604853256786, 0.3676719239173477, -0.36139654819928557),\n\
 vec3(-0.5773502691896258, -0.5773502691896258, 0.5773502691896258),\n\
 vec3(-0.6738873386790492, -0.6738873386790492, 0.30290544652768625),\n\
 vec3(-0.8568604853256786, -0.3676719239173477, 0.36139654819928557),\n\
 vec3(-0.7071067811865476, -0.7071067811865476, 0.0),\n\
 vec3(-0.9238795325112867, -0.3826834323650898, 0.0),\n\
 vec3(-0.9238795325112867, 0.0, 0.3826834323650898),\n\
 vec3(-0.8568604853256786, 0.3676719239173477, 0.36139654819928557),\n\
 vec3(-1.0, 0.0, 0.0),\n\
 vec3(-0.9238795325112867, 0.3826834323650898, 0.0),\n\
 vec3(-0.6738873386790492, -0.6738873386790492, -0.30290544652768625),\n\
 vec3(-0.8568604853256786, -0.3676719239173477, -0.36139654819928557),\n\
 vec3(-0.5773502691896258, -0.5773502691896258, -0.5773502691896258),\n\
 vec3(-0.9238795325112867, 0.0, -0.3826834323650898),\n\
 vec3(-0.8568604853256786, 0.3676719239173477, -0.36139654819928557),\n\
 vec3(-0.6738873386790492, 0.6738873386790492, 0.30290544652768625),\n\
 vec3(-0.3676719239173477, 0.8568604853256786, 0.36139654819928557),\n\
 vec3(-0.7071067811865476, 0.7071067811865476, 0.0),\n\
 vec3(-0.3826834323650898, 0.9238795325112867, 0.0),\n\
 vec3(0.0, 0.9238795325112867, 0.3826834323650898),\n\
 vec3(0.3676719239173477, 0.8568604853256786, 0.36139654819928557),\n\
 vec3(0.0, 1.0, 0.0),\n\
 vec3(0.3826834323650898, 0.9238795325112867, 0.0),\n\
 vec3(-0.6738873386790492, 0.6738873386790492, -0.30290544652768625),\n\
 vec3(-0.3676719239173477, 0.8568604853256786, -0.36139654819928557),\n\
 vec3(0.0, 0.9238795325112867, -0.3826834323650898),\n\
 vec3(0.3676719239173477, 0.8568604853256786, -0.36139654819928557),\n\
 vec3(-0.30290544652768625, -0.6738873386790492, 0.6738873386790492),\n\
 vec3(0.0, -0.7071067811865476, 0.7071067811865476),\n\
 vec3(-0.3676719239173477, -0.8568604853256786, 0.36139654819928557),\n\
 vec3(0.0, -0.9238795325112867, 0.3826834323650898),\n\
 vec3(0.30290544652768625, -0.6738873386790492, 0.6738873386790492),\n\
 vec3(0.3676719239173477, -0.8568604853256786, 0.36139654819928557),\n\
 vec3(-0.3826834323650898, -0.9238795325112867, 0.0),\n\
 vec3(0.0, -1.0, 0.0),\n\
 vec3(-0.3676719239173477, -0.8568604853256786, -0.36139654819928557),\n\
 vec3(0.0, -0.9238795325112867, -0.3826834323650898),\n\
 vec3(0.3826834323650898, -0.9238795325112867, 0.0),\n\
 vec3(0.3676719239173477, -0.8568604853256786, -0.36139654819928557)\n\
);\n\
uniform mat4 vp; \n\
uniform vec4 ssao_radius_move_start_end;\n\
\n\
uniform sampler2D position_depth;\n\
uniform sampler2D normal_ao;\n\
\n\
in vec2 out_uv;\n\
\n\
out float ssao_value;\n\
\n\
void main() {\n\
    vec3 pos = texture(position_depth, out_uv).xyz;\n\
    vec3 nrm = texture(normal_ao, out_uv).xyz;\n\
    ssao_value = 0.0;\n\
    for(int i = 0; i < ssao_samples_count; ++i) {\n\
        vec3 smp = ssao_samples[i];\n\
        smp *= ((1.5 * step(0.0, dot(nrm, smp)) - 0.5) * ssao_radius_move_start_end.x) + (nrm * ssao_radius_move_start_end.y);\n\
        smp += pos;\n\
        vec4 p = vp * vec4(smp, 1.0);\n\
        smp = p.xyz / p.w;\n\
        smp *= 0.5;\n\
        smp += 0.5;\n\
        float coverage = smp.z - texture(position_depth, smp.xy).w;\n\
        ssao_value += step(coverage, ssao_radius_move_start_end.z) + step(ssao_radius_move_start_end.w, coverage);\n\
    }\n\
    ssao_value = smoothstep(0.0, 1.0, ssao_value * ssao_samples_weight);\n\
}\n";
}

gearoenix::gl::shader::SsaoResolve::SsaoResolve(Engine& e)
    : Shader(e)
{
    set_vertex_shader(vertex_shader_src);
    set_fragment_shader(fragment_shader_src);
    link();
    GX_GL_SHADER_SET_TEXTURE_INDEX_STARTING;
    GX_GL_THIS_GET_UNIFORM(vp);
    GX_GL_THIS_GET_UNIFORM(ssao_radius_move_start_end);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(position_depth);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(normal_ao);
}

gearoenix::gl::shader::SsaoResolve::~SsaoResolve() = default;

void gearoenix::gl::shader::SsaoResolve::bind(uint& current_shader) const
{
    if (shader_program == current_shader)
        return;
    Shader::bind(current_shader);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(position_depth);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(normal_ao);
}

#endif