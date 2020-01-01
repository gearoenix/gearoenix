#include "glc3-shd-shadow-mapper.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-loader.hpp"
#include "../engine/glc3-eng-engine.hpp"
#include <sstream>

gearoenix::glc3::shader::ShadowMapper::ShadowMapper(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Shader(e, c)
{
    GX_GLC3_SHADER_SRC_DEFAULT_VERTEX_STARTING <<
        // effect uniform(s)
        "uniform mat4 effect_mvp;\n"
        // output(s)
        "out vec2 out_uv;\n"
        // Main function
        "void main()\n"
        "{\n"
        "    out_uv = uv;\n"
        "    gl_Position = effect_mvp * vec4(position, 1.0);\n"
        "}";
    GX_GLC3_SHADER_SRC_DEFAULT_FRAGMENT_STARTING << "uniform float effect_alpha;\n"
                                                    "uniform float effect_alpha_cutoff;\n"
                                                    "uniform sampler2D material_color;\n"
                                                    "in vec2 out_uv;\n"
                                                    "void main()\n"
                                                    "{\n"
                                                    "    vec4 tmp_v4 = texture(material_base_color, out_uv);\n"
                                                    "    tmp_v4.w *= effect_alpha;\n"
                                                    "    if(tmp_v4.w < effect_alpha_cutoff) discard;\n"
                                                    "}";
    e->get_function_loader()->load([this, vertex_shader_code { vertex_shader_code.str() }, fragment_shader_code { fragment_shader_code.str() }] {
        set_vertex_shader(vertex_shader_code);
        set_fragment_shader(fragment_shader_code);
        link();
        GX_GLC3_SHADER_SET_TEXTURE_INDEX_STARTING
        GX_GLC3_THIS_GET_UNIFORM(effect_alpha)
        GX_GLC3_THIS_GET_UNIFORM(effect_alpha_cutoff)
        GX_GLC3_THIS_GET_UNIFORM_TEXTURE(material_color)
        GX_GLC3_THIS_GET_UNIFORM(effect_mvp)
    });
}

gearoenix::glc3::shader::ShadowMapper::~ShadowMapper() noexcept = default;

void gearoenix::glc3::shader::ShadowMapper::bind() const noexcept
{
    Shader::bind();
    GX_GLC3_SHADER_SET_TEXTURE_INDEX_UNIFORM(material_color)
}

#endif
