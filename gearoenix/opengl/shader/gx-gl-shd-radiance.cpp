#include "gx-gl-shd-radiance.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../gx-gl-engine.hpp"

namespace {
constexpr char vertex_shader_body[] = R"SHADER(
layout(location = 0) in vec2 position;

uniform mat3 m;

out vec3 out_pos;

void main() {
    out_pos = m * vec3(position, 1.0);
    gl_Position = vec4(position, 0.0, 1.0);
}
)SHADER";

constexpr char fragment_shader_body[] = R"SHADER(
uniform float roughness;
uniform float roughness_p_4;
uniform float sa_texel;

uniform samplerCube environment;

in vec3 out_pos;

out vec4 frag_out;

float distribution_ggx(float NdotH2) {
    float nom   = roughness_p_4;
    float denom = (NdotH2 * (roughness_p_4 - 1.0) + 1.0);
    denom = gx_pi * denom * denom;
    return nom / denom;
}

float radical_inverse_v_d_c(uint bits) {
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10;
}

vec2 hammersley(uint i, uint N) {
    return vec2(float(i)/float(N), radical_inverse_v_d_c(i));
}

vec3 importance_sample_ggx(vec2 xi, vec3 n) {
    float phi = 2.0 * gx_pi * xi.x;
    float cos_theta = sqrt((1.0 - xi.y) / (1.0 + (roughness_p_4 - 1.0) * xi.y));
    float sin_theta = sqrt(1.0 - cos_theta * cos_theta);
    vec3 h;
    h.x = cos(phi) * sin_theta;
    h.y = sin(phi) * sin_theta;
    h.z = cos_theta;
    vec3 up = abs(n.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, n));
    vec3 bitangent = cross(n, tangent);
    vec3 sample_vec = tangent * h.x + bitangent * h.y + n * h.z;
    return normalize(sample_vec);
}

void main() {
    vec3 nrm = normalize(out_pos);
    const uint SAMPLE_COUNT = 1024u;
    vec3 prefiltered_colour = vec3(0.0);
    float total_weight = 0.0;
    for(uint i = uint(0); i < SAMPLE_COUNT; i += uint(1)) {
        vec2 xi = hammersley(i, SAMPLE_COUNT);
        vec3 h = importance_sample_ggx(xi, nrm);
        float n_dot_h = dot(nrm, h);
        vec3 l = normalize(2.0 * n_dot_h * h - nrm);
        float n_dot_l = dot(nrm, l);
        if(n_dot_l > 0.0) {
            n_dot_h = max(n_dot_h, 0.0);
            float n_dot_h_p_2 = n_dot_h * n_dot_h;
            float d = distribution_ggx(n_dot_h_p_2);
            float pdf = d * n_dot_h / (4.0 * n_dot_h) + 0.0001;
            float sa_sample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);
            float mip_level = roughness == 0.0 ? 0.0 : 0.5 * log2(sa_sample / sa_texel);
            prefiltered_colour += textureLod(environment, l, mip_level).rgb * n_dot_l;
            total_weight += n_dot_l;
        }
    }
    prefiltered_colour = prefiltered_colour / total_weight;
    frag_out = vec4(prefiltered_colour, 1.0);
}
)SHADER";
}

gearoenix::gl::shader::Radiance::Radiance()
{
    set_vertex_shader(get_common_shader_starter() + vertex_shader_body);
    set_fragment_shader(get_common_shader_starter() + fragment_shader_body);
    link();
    GX_GL_SHADER_SET_TEXTURE_INDEX_STARTING;
    GX_GL_THIS_GET_UNIFORM(roughness);
    GX_GL_THIS_GET_UNIFORM(roughness_p_4);
    GX_GL_THIS_GET_UNIFORM(sa_texel);
    GX_GL_THIS_GET_UNIFORM(m);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(environment);
}

gearoenix::gl::shader::Radiance::~Radiance() = default;

void gearoenix::gl::shader::Radiance::bind(uint& current_shader) const
{
    if (shader_program == current_shader)
        return;
    Shader::bind(current_shader);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(environment);
}

#endif