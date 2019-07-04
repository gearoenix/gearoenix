#include "gles2-shd-shadow-mapper.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../system/sys-log.hpp"
#include "../engine/gles2-eng-engine.hpp"

const static std::string vertex_shader_code = GX_GLES2_SHADER_SRC_DEFAULT_VERTEX_STARTING
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

const static std::string fragment_shader_code = GX_GLES2_SHADER_SRC_DEFAULT_FRAGMENT_STARTING
    GX_GLES2_SHADER_SRC_MATERIAL_RESOURCES
    "varying vec2 out_depth;\n"
    "varying vec2 out_uv;\n"
    "void main()\n"
    "{\n"
    "    vec4 tmpv4 = texture2D(material_base_color, out_uv);\n"
    "    tmpv4.w *= material_alpha;\n"
    "    if(tmpv4.w < material_alpha_cutoff) discard;\n"
    "    gl_FragColor = vec4(fract(out_depth), 0.0, 1.0);\n"
    "}";

gearoenix::gles2::shader::ShadowMapper::ShadowMapper(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Shader(e, c)
{
    e->get_function_loader()->load([this] {
        set_vertex_shader(vertex_shader_code);
        set_fragment_shader(fragment_shader_code);
        link();
        GX_GLES2_SHADER_SET_TEXTURE_INDEX_STARTING
        GX_GLES2_SHADER_MATERIAL_GET_UNIFORM_LOCATIONS
        GX_GLES2_THIS_GET_UNIFORM_F(effect_mvp);
    });
}

gearoenix::gles2::shader::ShadowMapper::~ShadowMapper() noexcept
{
}

void gearoenix::gles2::shader::ShadowMapper::bind() const noexcept
{
    Shader::bind();
    GX_GLES2_SHADER_MATERIAL_SET_TEXTURE_INDEX_UNIFORM
}

#endif
