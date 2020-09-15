#include "gx-gles2-shd-unlit.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/gx-cr-function-loader.hpp"
#include "../../gl/gx-gl-loader.hpp"
#include "../engine/gx-gles2-eng-engine.hpp"

gearoenix::gles2::shader::Unlit::Unlit(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Shader(e, c)
{
    e->get_function_loader()->load([this] {
        const std::string vertex_shader_code = GX_GLES2_SHADER_SRC_DEFAULT_VERTEX_STARTING
            // effect uniform(s)
            "uniform mat4 effect_mvp;\n"
            // output(s)
            "varying vec2 out_uv;\n"
            // Main function
            "void main()\n"
            "{\n"
            "    out_uv = uv;\n"
            "    gl_Position = effect_mvp * vec4(position, 1.0);\n"
            "}";

        const std::string fragment_shader_code = GX_GLES2_SHADER_SRC_DEFAULT_FRAGMENT_STARTING
            "uniform float     material_alpha;\n"
            "uniform float     material_alpha_cutoff;\n"
            "uniform sampler2D material_color;\n"
            "varying vec2      out_uv;\n"
            "void main()\n"
            "{\n"
            "    vec4 temp_v4 = texture2D(material_color, out_uv);\n"
            "    temp_v4.w *= material_alpha;\n"
            "    if(temp_v4.w < material_alpha_cutoff) discard;\n"
            "    gl_FragColor = temp_v4;\n"
            "}";
        set_vertex_shader(vertex_shader_code);
        set_fragment_shader(fragment_shader_code);
        link();
        GX_GLES2_SHADER_SET_TEXTURE_INDEX_STARTING
        GX_GLES2_THIS_GET_UNIFORM(material_alpha)
        GX_GLES2_THIS_GET_UNIFORM(material_alpha_cutoff)
        GX_GLES2_THIS_GET_UNIFORM_TEXTURE(material_color)
        GX_GLES2_THIS_GET_UNIFORM(effect_mvp)
    });
}

gearoenix::gles2::shader::Unlit::~Unlit() noexcept = default;

void gearoenix::gles2::shader::Unlit::bind() const noexcept
{
    Shader::bind();
    GX_GLES2_SHADER_SET_TEXTURE_INDEX_UNIFORM(material_color)
}
#endif
