#include "gles2-shd-irradiance-convoluter.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-loader.hpp"
#include "../engine/gles2-eng-engine.hpp"
#include <sstream>

gearoenix::gles2::shader::IrradianceConvoluter::IrradianceConvoluter(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Shader(e, c)
{
    const std::string vertex_shader_code = GX_GLES2_SHADER_SRC_DEFAULT_VERTEX_STARTING
        // output
        "varying vec3 out_pos;\n"
        // Main function
        "void main()\n"
        "{\n"
        "    out_pos = normal;\n" // normal in here is world pos of the face vertex
        "    gl_Position = vec4(position, 1.0);\n"
        "}";
    const std::string fragment_shader_code = GX_GLES2_SHADER_SRC_DEFAULT_FRAGMENT_STARTING
        // input
        "varying vec3 out_pos;\n"
        "uniform samplerCube environment;\n"
        "void main()\n"
        "{\n"
        "    vec3 nrm = normalize(out_pos);\n" // TODO move this up right thing in to vertex shader (if it was possible)
        "    vec3 irradiance = vec3(0.0);\n"
        "    vec3 up = vec3(0.0, 1.0, 0.0);\n"
        "    vec3 right = cross(up, nrm);\n"
        "    up = cross(nrm, right);\n"
        "    float sample_delta = 0.025;\n"
        "    float samples_count = 0.0;\n"
        "    for(float phi = 0.0; phi < 2.0 * GX_PI; phi += sample_delta) {\n"
        "        for(float theta = 0.0; theta < 0.5 * GX_PI; theta += sample_delta, ++samples_count) {\n"
        "            vec3 tangent_sample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));\n"
        "            vec3 sample_vec = tangent_sample.x * right + tangent_sample.y * up + tangent_sample.z * nrm;\n"
        "            irradiance += textureCube(environment, sample_vec).rgb * cos(theta) * sin(theta);\n"
        "        }\n"
        "    }\n"
        "    irradiance *= GX_PI / samples_count;\n"
        "    gl_FragColor = vec4(irradiance, 1.0);\n"
        "}";
    e->get_function_loader()->load([this, vertex_shader_code, fragment_shader_code] {
        set_vertex_shader(vertex_shader_code);
        set_fragment_shader(fragment_shader_code);
        link();
        GX_GLES2_SHADER_SET_TEXTURE_INDEX_STARTING
        GX_GLES2_THIS_GET_UNIFORM_TEXTURE(environment)
    });
}

gearoenix::gles2::shader::IrradianceConvoluter::~IrradianceConvoluter() noexcept = default;

void gearoenix::gles2::shader::IrradianceConvoluter::bind() const noexcept
{
    Shader::bind();
    GX_GLES2_SHADER_SET_TEXTURE_INDEX_UNIFORM(environment)
}

#endif
