#include "gx-gl-shd-multiply.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED

namespace {
constexpr char vertex_shader_body[] = R"SHADER(
layout(location = 0) in vec2 position;

out vec2 out_uv;

void main() {
    gl_Position = vec4(position, 0.0, 1.0);
    out_uv = position* 0.5 + 0.5;
}
)SHADER";

constexpr char fragment_shader_body[] = R"SHADER(
uniform vec4 value_mip_index;
uniform sampler2D source_texture;

in vec2 out_uv;

layout(location = 0) out vec4 frag_colour;

void main() {
    frag_colour = textureLod(source_texture, out_uv, value_mip_index.w);
    frag_colour.xyz *= value_mip_index.xyz;
}
)SHADER";
}

gearoenix::gl::shader::Multiply::Multiply()
{
    set_vertex_shader(get_common_shader_starter() + vertex_shader_body);
    set_fragment_shader(get_common_shader_starter() + fragment_shader_body);

    link();
    GX_GL_SHADER_SET_TEXTURE_INDEX_STARTING;
    GX_GL_THIS_GET_UNIFORM(value_mip_index);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(source_texture);
}

gearoenix::gl::shader::Multiply::~Multiply() = default;

void gearoenix::gl::shader::Multiply::bind(uint& current_shader) const
{
    if (shader_program == current_shader) {
        return;
    }
    Shader::bind(current_shader);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(source_texture);
}
#endif