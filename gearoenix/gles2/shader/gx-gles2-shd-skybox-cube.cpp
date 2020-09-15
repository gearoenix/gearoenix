#include "gx-gles2-shd-skybox-cube.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/gx-cr-function-loader.hpp"
#include "../../gl/gx-gl-loader.hpp"
#include "../engine/gx-gles2-eng-engine.hpp"

gearoenix::gles2::shader::SkyboxCube::SkyboxCube(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Shader(e, c)
{
    e->get_function_loader()->load([this] {
        const std::string vertex_shader_code = GX_GLES2_SHADER_SRC_DEFAULT_VERTEX_STARTING
            // effect uniform(s)
            "uniform mat4 effect_mvp;\n"
            // output(s)
            "varying vec3 out_pos;\n"
            // Main function
            "void main()\n"
            "{\n"
            "    out_pos = position;\n"
            "    gl_Position = effect_mvp * vec4(position, 1.0);\n"
            "}";

        const std::string fragment_shader_code = GX_GLES2_SHADER_SRC_DEFAULT_FRAGMENT_STARTING
            "uniform float material_alpha;\n"
            "uniform float material_alpha_cutoff;\n"
            "uniform samplerCube material_color;\n"
            "uniform float camera_hdr_tune_mapping;\n"
            "uniform float camera_gamma_correction;\n"
            "varying vec3 out_pos;\n"
            "void main()\n"
            "{\n"
            "    vec4 tmp_v4 = textureCube(material_color, out_pos);\n"
            "    tmp_v4.w *= material_alpha;\n"
            "    if(tmp_v4.w < material_alpha_cutoff) discard;\n"
            "    if(camera_gamma_correction > 0.001) {\n"
            "        tmp_v4.xyz = pow(tmp_v4.xyz / (tmp_v4.xyz + camera_hdr_tune_mapping), vec3(1.0 / camera_gamma_correction));\n"
            "    }\n"
            "    gl_FragColor = tmp_v4;\n"
            "}";
        set_vertex_shader(vertex_shader_code);
        set_fragment_shader(fragment_shader_code);
        link();
        GX_GLES2_SHADER_SET_TEXTURE_INDEX_STARTING
        GX_GLES2_THIS_GET_UNIFORM(material_alpha)
        GX_GLES2_THIS_GET_UNIFORM(material_alpha_cutoff)
        GX_GLES2_THIS_GET_UNIFORM_TEXTURE(material_color)
        GX_GLES2_THIS_GET_UNIFORM(camera_hdr_tune_mapping)
        GX_GLES2_THIS_GET_UNIFORM(camera_gamma_correction)
        GX_GLES2_THIS_GET_UNIFORM(effect_mvp)
    });
}

gearoenix::gles2::shader::SkyboxCube::~SkyboxCube() noexcept = default;

void gearoenix::gles2::shader::SkyboxCube::bind() const noexcept
{
    Shader::bind();
    GX_GLES2_SHADER_SET_TEXTURE_INDEX_UNIFORM(material_color)
}
#endif
