#include "gles2-shd-shadow-mapper.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-loader.hpp"
#include "../engine/gles2-eng-engine.hpp"

gearoenix::gles2::shader::ShadowMapper::ShadowMapper(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Shader(e, c)
{
    e->get_function_loader()->load([this] {
        const std::string vertex_shader_code = GX_GLES2_SHADER_SRC_DEFAULT_VERTEX_STARTING
            // effect uniform(s)
            "uniform mat4 effect_mvp;\n"
            // output(s)
            "varying vec2 out_depth;\n"
            "varying vec2 out_uv;\n"
            // Main function
            "void main()\n"
            "{\n"
            "    vec4 pos = effect_mvp * vec4(position, 1.0);\n"
            "    float depth = (pos.z / pos.w) * 0.5 + 0.5;\n"
            "    out_depth = vec2(depth, depth * 256.0);\n"
            "    out_uv = uv;\n"
            "    gl_Position = pos;\n"
            "}";

        const std::string fragment_shader_code = GX_GLES2_SHADER_SRC_DEFAULT_FRAGMENT_STARTING
            "uniform float     effect_alpha;\n"
            "uniform float     effect_alpha_cutoff;\n"
            "uniform sampler2D material_color;\n"
            "varying vec2      out_depth;\n"
            "varying vec2      out_uv;\n"
            "void main()\n"
            "{\n"
            "    vec4 temp_v4 = texture2D(material_color, out_uv);\n"
            "    temp_v4.w *= effect_alpha;\n"
            "    if(temp_v4.w < effect_alpha_cutoff) discard;\n"
            "    gl_FragColor = vec4(fract(out_depth), 0.0, 1.0);\n"
            "}";
        set_vertex_shader(vertex_shader_code);
        set_fragment_shader(fragment_shader_code);
        link();
        GX_GLES2_SHADER_SET_TEXTURE_INDEX_STARTING
        GX_GLES2_THIS_GET_UNIFORM_TEXTURE(material_color)
        GX_GLES2_THIS_GET_UNIFORM(effect_alpha)
        GX_GLES2_THIS_GET_UNIFORM(effect_alpha_cutoff)
        GX_GLES2_THIS_GET_UNIFORM(effect_mvp);
    });
}

gearoenix::gles2::shader::ShadowMapper::~ShadowMapper() noexcept = default;

void gearoenix::gles2::shader::ShadowMapper::bind() const noexcept
{
    Shader::bind();
    GX_GLES2_SHADER_SET_TEXTURE_INDEX_UNIFORM(material_color)
}

#endif
