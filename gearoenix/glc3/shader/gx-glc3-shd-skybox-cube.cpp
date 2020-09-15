#include "gx-glc3-shd-skybox-cube.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/gx-cr-function-loader.hpp"
#include "../../gl/gx-gl-loader.hpp"
#include "../engine/gx-glc3-eng-engine.hpp"
#include <sstream>

gearoenix::glc3::shader::SkyboxCube::SkyboxCube(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Shader(e, c)
{
    GX_GLC3_SHADER_SRC_DEFAULT_VERTEX_STARTING <<
        // effect uniform(s)
        "uniform mat4 effect_mvp;\n"
        // output(s)
        "out vec3 out_pos;\n"
        // Main function
        "void main()\n"
        "{\n"
        "    out_pos = position;\n"
        "    gl_Position = effect_mvp * vec4(position, 1.0);\n"
        "}";
    GX_GLC3_SHADER_SRC_DEFAULT_FRAGMENT_STARTING <<
        // material uniforms
        "uniform float material_alpha;\n"
        "uniform float material_alpha_cutoff;\n"
        "uniform samplerCube material_color;\n"
        "uniform float camera_hdr_tune_mapping;\n"
        "uniform float camera_gamma_correction;\n"
        "in vec3 out_pos;\n"
        "out vec4 frag_color;\n"
        "void main()\n"
        "{\n"
        "    vec4 tmp_v4 = texture(material_color, out_pos);\n"
        "    tmp_v4.w *= material_alpha;\n"
        "    if(tmp_v4.w < material_alpha_cutoff) discard;\n"
        "    if(camera_gamma_correction > 0.001) {\n"
        "        tmp_v4.xyz = pow(tmp_v4.xyz / (tmp_v4.xyz + camera_hdr_tune_mapping), vec3(1.0 / camera_gamma_correction));\n"
        "    }\n"
        "    frag_color = tmp_v4;\n"
        "}";
    e->get_function_loader()->load([this, vertex_shader_code { vertex_shader_code.str() }, fragment_shader_code { fragment_shader_code.str() }] {
        set_vertex_shader(vertex_shader_code);
        set_fragment_shader(fragment_shader_code);
        link();
        GX_GLC3_SHADER_SET_TEXTURE_INDEX_STARTING
        GX_GLC3_THIS_GET_UNIFORM(material_alpha)
        GX_GLC3_THIS_GET_UNIFORM(material_alpha_cutoff)
        GX_GLC3_THIS_GET_UNIFORM_TEXTURE(material_color)
        GX_GLC3_THIS_GET_UNIFORM(camera_hdr_tune_mapping)
        GX_GLC3_THIS_GET_UNIFORM(camera_gamma_correction)
        GX_GLC3_THIS_GET_UNIFORM(effect_mvp)
    });
}

gearoenix::glc3::shader::SkyboxCube::~SkyboxCube() noexcept = default;

void gearoenix::glc3::shader::SkyboxCube::bind() const noexcept
{
    Shader::bind();
    GX_GLC3_SHADER_SET_TEXTURE_INDEX_UNIFORM(material_color)
}

#endif
