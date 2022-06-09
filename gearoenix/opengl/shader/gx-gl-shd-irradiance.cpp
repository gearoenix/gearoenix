#include "gx-gl-shd-irradiance.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../core/macro/gx-cr-mcr-stringifier.hpp"
#include "../gx-gl-engine.hpp"

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
uniform samplerCube environment;\n\
\n\
in vec3 out_pos;\n\
\n\
out vec4 frag_out;\n\
\n\
void main() {\n\
    vec3 nrm = normalize(out_pos);\n\
    vec3 irradiance = vec3(0.0);\n\
    vec3 up = vec3(0.0, 1.0, 0.0);\n\
    vec3 right = normalize(cross(up, nrm));\n\
    up = normalize(cross(nrm, right));\n\
    float sample_delta = 0.005;\n\
    float samples_count = 0.0f;\n\
    for(float phi = 0.0; phi < 2.0 * GX_PI; phi += sample_delta) {\n\
        for(float theta = 0.0; theta < 0.5 * GX_PI; theta += sample_delta, ++samples_count) {\n\
            vec3 tangent_sample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));\n\
            vec3 sample_vec = tangent_sample.x * right + tangent_sample.y * up + tangent_sample.z * nrm;\n\
            irradiance += texture(environment, normalize(sample_vec)).rgb * cos(theta) * sin(theta);\n\
        }\n\
    }\n\
    irradiance *= GX_PI / samples_count;\n\
    frag_out = vec4(irradiance, 1.0);\n\
}\n";

gearoenix::gl::shader::Irradiance::Irradiance(Engine& e) noexcept
    : Shader(e)
{
    set_vertex_shader(vertex_shader_src);
    set_fragment_shader(fragment_shader_src);
    link();
    GX_GL_SHADER_SET_TEXTURE_INDEX_STARTING;
    GX_GL_THIS_GET_UNIFORM(m);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(environment);
}

gearoenix::gl::shader::Irradiance::~Irradiance() noexcept = default;

void gearoenix::gl::shader::Irradiance::bind() const noexcept
{
    Shader::bind();
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(environment);
}

#endif