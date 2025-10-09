#include "gx-gl-shd-final.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED

namespace {
constexpr auto vertex_shader_body = R"SHADER(
layout(location = 0) in vec2 position;

out vec2 out_uv;

void main() {
    gl_Position = vec4(position, 0.0, 1.0);
    out_uv = position* 0.5 + 0.5;
}
)SHADER";

constexpr auto fragment_shader_body = R"SHADER(
uniform sampler2D albedo;

in vec2 out_uv;

out vec4 frag_colour;

void main() {
    frag_colour = textureLod(albedo, out_uv, 0.0);
}
)SHADER";
}

gearoenix::gl::shader::Final::Final()
{
    auto vertex_shader_src = get_common_shader_starter() + vertex_shader_body;
    set_vertex_shader(vertex_shader_src);

    auto fragment_shader_src = get_common_shader_starter() + fragment_shader_body;
    set_fragment_shader(fragment_shader_src);

    link();
    GX_GL_SHADER_SET_TEXTURE_INDEX_STARTING;
    GX_GL_THIS_GET_UNIFORM_TEXTURE(albedo);
}

gearoenix::gl::shader::Final::~Final() = default;

void gearoenix::gl::shader::Final::bind(uint& current_shader) const
{
    if (shader_program == current_shader) {
        return;
    }
    Shader::bind(current_shader);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(albedo);
}

#endif