#include "glc3-shd-shadow-mapper.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../system/sys-log.hpp"
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
    GX_GLC3_SHADER_SRC_DEFAULT_FRAGMENT_STARTING << GX_GLC3_SHADER_SRC_MATERIAL_RESOURCES
        "in vec2 out_uv;\n"
        "void main()\n"
        "{\n"
        "    vec4 tmpv4 = texture(material_base_color, out_uv);\n"
        "    tmpv4.w *= material_alpha;\n"
        "    if(tmpv4.w < material_alpha_cutoff) discard;\n"
        "}";
    e->get_function_loader()->load([this, vertex_shader_code { move(vertex_shader_code.str()) }, fragment_shader_code { move(fragment_shader_code.str()) }] {
        set_vertex_shader(vertex_shader_code);
        set_fragment_shader(fragment_shader_code);
        link();
        GX_GLC3_SHADER_SET_TEXTURE_INDEX_STARTING
        GX_GLC3_SHADER_MATERIAL_GET_UNIFORM_LOCATIONS
        GX_GLC3_THIS_GET_UNIFORM_F(effect_mvp);
    });
}

gearoenix::glc3::shader::ShadowMapper::~ShadowMapper() noexcept
{
}

void gearoenix::glc3::shader::ShadowMapper::bind() const noexcept
{
    Shader::bind();
    GX_GLC3_SHADER_MATERIAL_SET_TEXTURE_INDEX_UNIFORM
}

#endif
