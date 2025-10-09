#include "gx-gl-shd-irradiance.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../gx-gl-engine.hpp"

namespace {
constexpr auto vertex_shader_body = R"SHADER(
layout(location = 0) in vec2 position;

uniform mat3 m;

out vec3 out_pos;

void main() {
    out_pos = m * vec3(position, 1.0);
    gl_Position = vec4(position, 0.0, 1.0);
}
)SHADER";

constexpr auto fragment_shader_body = R"SHADER(
uniform samplerCube environment;

in vec3 out_pos;

out vec4 frag_out;

void main() {
    vec3 nrm = normalize(out_pos);
    vec3 irradiance = vec3(0.0);
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, nrm));
    up = normalize(cross(nrm, right));
    float sample_delta = 0.005;
    float phi_samples_count = 0.001f;
    for(float phi = 0.0; phi < 2.0 * gx_pi; phi += sample_delta, ++phi_samples_count) {
        float theta_samples_count = 0.001f;
        vec3 irradiance_temp = vec3(0.0);
        for(float theta = 0.0; theta < 0.5 * gx_pi; theta += sample_delta, ++theta_samples_count) {
            vec3 tangent_sample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            vec3 sample_vec = tangent_sample.x * right + tangent_sample.y * up + tangent_sample.z * nrm;
            irradiance_temp += texture(environment, normalize(sample_vec)).rgb * cos(theta) * sin(theta);
        }
        irradiance += irradiance_temp / theta_samples_count;
    }
    irradiance *= gx_pi / phi_samples_count;
    frag_out = vec4(irradiance, 1.0);
}
)SHADER";
}

gearoenix::gl::shader::Irradiance::Irradiance()
{
    const auto vertex_shader_src = get_common_shader_starter() + vertex_shader_body;
    set_vertex_shader(vertex_shader_src);

    const auto fragment_shader_src = get_common_shader_starter() + fragment_shader_body;
    set_fragment_shader(fragment_shader_src);

    link();
    GX_GL_SHADER_SET_TEXTURE_INDEX_STARTING;
    GX_GL_THIS_GET_UNIFORM(m);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(environment);
}

gearoenix::gl::shader::Irradiance::~Irradiance() = default;

void gearoenix::gl::shader::Irradiance::bind(uint& current_shader) const
{
    if (shader_program == current_shader) {
        return;
    }
    Shader::bind(current_shader);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(environment);
}

#endif