#include "gx-gl-shader-radiance.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/macro/gx-cr-mcr-stringifier.hpp"
#include "gx-gl-engine.hpp"

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
uniform mat3 m;\n\
\n\
out vec3 out_pos;\n\
\n\
void main() {\n\
    out_pos = m * vec3(position, 1.0);\n\
    gl_Position = vec4(position, 0.0, 1.0);\n\
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
uniform float roughness; \n\
uniform float roughness_p_4;\n\
uniform float sa_texel;\n\
\n\
uniform samplerCube environment;\n\
\n\
in vec3 out_pos;\n\
\n\
out vec4 frag_out;\n\
\n\
float distribution_ggx(float NdotH2) {\n\
\n\
    float nom   = roughness_p_4;\n\
    float denom = (NdotH2 * (roughness_p_4 - 1.0) + 1.0);\n\
    denom = GX_PI * denom * denom;\n\
    return nom / denom;\n\
}\n\
\n\
float radical_inverse_v_d_c(uint bits) {\n\
    bits = (bits << 16u) | (bits >> 16u);\n\
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);\n\
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);\n\
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);\n\
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);\n\
    return float(bits) * 2.3283064365386963e-10;\n\
}\n\
\n\
vec2 hammersley(uint i, uint N) {\n\
    return vec2(float(i)/float(N), radical_inverse_v_d_c(i));\n\
}\n\
\n\
vec3 importance_sample_ggx(vec2 xi, vec3 n) {\n\
    float phi = 2.0 * GX_PI * xi.x;\n\
    float cos_theta = sqrt((1.0 - xi.y) / (1.0 + (roughness_p_4 - 1.0) * xi.y));\n\
    float sin_theta = sqrt(1.0 - cos_theta * cos_theta);\n\
    vec3 h;\n\
    h.x = cos(phi) * sin_theta;\n\
    h.y = sin(phi) * sin_theta;\n\
    h.z = cos_theta;\n\
    vec3 up = abs(n.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);\n\
    vec3 tangent = normalize(cross(up, n));\n\
    vec3 bitangent = cross(n, tangent);\n\
    vec3 sample_vec = tangent * h.x + bitangent * h.y + n * h.z;\n\
    return normalize(sample_vec);\n\
}\n\
\n\
void main() {\n\
    vec3 nrm = normalize(out_pos);\n\
    const uint SAMPLE_COUNT = 1024u;\n\
    vec3 prefiltered_colour = vec3(0.0);\n\
    float total_weight = 0.0;\n\
    for(uint i = uint(0); i < SAMPLE_COUNT; i += uint(1)) {\n\
        vec2 xi = hammersley(i, SAMPLE_COUNT);\n\
        vec3 h = importance_sample_ggx(xi, nrm);\n\
        float n_dot_h = dot(nrm, h);\n\
        vec3 l = normalize(2.0 * n_dot_h * h - nrm);\n\
        float n_dot_l = dot(nrm, l);\n\
        if(n_dot_l > 0.0) {\n\
            n_dot_h = max(n_dot_h, 0.0);\n\
            float n_dot_h_p_2 = n_dot_h * n_dot_h;\n\
            float d = distribution_ggx(n_dot_h_p_2);\n\
            float pdf = d * n_dot_h / (4.0 * n_dot_h) + 0.0001;\n\
            float sa_sample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);\n\
            float mip_level = roughness == 0.0 ? 0.0 : 0.5 * log2(sa_sample / sa_texel);\n\
            prefiltered_colour += textureLod(environment, l, mip_level).rgb * n_dot_l;\n\
            total_weight += n_dot_l;\n\
        }\n\
    }\n\
    prefiltered_colour = prefiltered_colour / total_weight;\n\
    frag_out = vec4(prefiltered_colour, 1.0);\n\
}\n";

gearoenix::gl::ShaderRadiance::ShaderRadiance(Engine& e) noexcept
    : Shader(e)
{
    set_vertex_shader(vertex_shader_src);
    set_fragment_shader(fragment_shader_src);
    link();
    GX_GL_SHADER_SET_TEXTURE_INDEX_STARTING
    GX_GL_THIS_GET_UNIFORM(roughness)
    GX_GL_THIS_GET_UNIFORM(roughness_p_4)
    GX_GL_THIS_GET_UNIFORM(sa_texel)
    GX_GL_THIS_GET_UNIFORM(m)
    GX_GL_THIS_GET_UNIFORM_TEXTURE(environment)
}

gearoenix::gl::ShaderRadiance::~ShaderRadiance() noexcept = default;

void gearoenix::gl::ShaderRadiance::bind() const noexcept
{
    Shader::bind();
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(environment)
}

#endif