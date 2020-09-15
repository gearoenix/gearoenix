#include "gx-glc3-shd-irradiance-convoluter.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/gx-cr-function-loader.hpp"
#include "../../gl/gx-gl-loader.hpp"
#include "../engine/gx-glc3-eng-engine.hpp"
#include <sstream>

gearoenix::glc3::shader::IrradianceConvoluter::IrradianceConvoluter(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Shader(e, c)
{
    GX_GLC3_SHADER_SRC_DEFAULT_VERTEX_STARTING <<
        // output
        "out vec3 out_pos;\n"
        "uniform mat4 mvp;\n"
        // Main function
        "void main()\n"
        "{\n"
        "    out_pos = position;\n"
        "    gl_Position = mvp * vec4(position, 1.0);\n"
        "}";
    GX_GLC3_SHADER_SRC_DEFAULT_FRAGMENT_STARTING <<
        // input
        "in vec3 out_pos;\n"
        "out vec4 frag_color;\n"
        "uniform samplerCube environment;\n"
        "void main()\n"
        "{\n"
        "    vec3 nrm = normalize(out_pos);\n"
        "    vec3 irradiance = vec3(0.0);\n"
        "    vec3 up = vec3(0.0, 1.0, 0.0);\n"
        "    vec3 right = normalize(cross(up, nrm));\n"
        "    up = normalize(cross(nrm, right));\n"
        "    float sample_delta = 0.025;\n"
        "    float samples_count = 0.0f;\n"
        "    for(float phi = 0.0; phi < 2.0 * GX_PI; phi += sample_delta) {\n"
        "        for(float theta = 0.0; theta < 0.5 * GX_PI; theta += sample_delta, ++samples_count) {\n"
        "            vec3 tangent_sample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));\n"
        "            vec3 sample_vec = tangent_sample.x * right + tangent_sample.y * up + tangent_sample.z * nrm;\n"
        "            irradiance += texture(environment, normalize(sample_vec)).rgb * cos(theta) * sin(theta);\n"
        "        }\n"
        "    }\n"
        "    irradiance *= GX_PI / samples_count;\n"
        "    irradiance.x = (irradiance.x <= 0.0 || irradiance.x >= 0.0)? irradiance.x: 0.0;\n"
        "    irradiance.y = (irradiance.y <= 0.0 || irradiance.y >= 0.0)? irradiance.y: 0.0;\n"
        "    irradiance.z = (irradiance.z <= 0.0 || irradiance.z >= 0.0)? irradiance.z: 0.0;\n"
        "    frag_color = vec4(irradiance, 1.0);\n"
        "}";
    e->get_function_loader()->load([this, vertex_shader_code { vertex_shader_code.str() }, fragment_shader_code { fragment_shader_code.str() }] {
        set_vertex_shader(vertex_shader_code);
        set_fragment_shader(fragment_shader_code);
        link();
        GX_GLC3_SHADER_SET_TEXTURE_INDEX_STARTING
        GX_GLC3_THIS_GET_UNIFORM_TEXTURE(environment)
        GX_GLC3_THIS_GET_UNIFORM(mvp)
    });
}

gearoenix::glc3::shader::IrradianceConvoluter::~IrradianceConvoluter() noexcept = default;

void gearoenix::glc3::shader::IrradianceConvoluter::bind() const noexcept
{
    Shader::bind();
    GX_GLC3_SHADER_SET_TEXTURE_INDEX_UNIFORM(environment)
}

#endif
