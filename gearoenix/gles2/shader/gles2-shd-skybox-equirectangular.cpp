#include "gles2-shd-skybox-equirectangular.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-loader.hpp"
#include "../engine/gles2-eng-engine.hpp"

gearoenix::gles2::shader::SkyboxEquirectangular::SkyboxEquirectangular(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
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
            "uniform float     material_alpha;\n"
            "uniform float     material_alpha_cutoff;\n"
            "uniform sampler2D material_color;\n"
            "varying vec3      out_pos;\n"
            "const vec2 inv_atan = vec2(0.1591, 0.3183);\n"
            "void main()\n"
            "{\n"
            "    vec4 temp_v4 = vec4(normalize(out_pos), 0.0);\n"
            "    temp_v4.xy = vec2(atan(temp_v4.y, temp_v4.x), asin(-temp_v4.z)) * inv_atan + 0.5;\n"
            "    temp_v4 = texture2D(material_color, temp_v4.xy);\n"
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

gearoenix::gles2::shader::SkyboxEquirectangular::~SkyboxEquirectangular() noexcept = default;

void gearoenix::gles2::shader::SkyboxEquirectangular::bind() const noexcept
{
    Shader::bind();
    GX_GLES2_SHADER_SET_TEXTURE_INDEX_UNIFORM(material_color)
}
#endif
