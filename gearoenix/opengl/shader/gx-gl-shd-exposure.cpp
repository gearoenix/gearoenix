#include "gx-gl-shd-exposure.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED

static constexpr const char* const vertex_shader_src = "\
#version 300 es\n\
\n\
precision highp float;\n\
\n\
layout(location = 0) in vec2 position;\n\
\n\
out vec2 out_uv;\n\
\n\
void main() {\n\
    gl_Position = vec4(position, 0.0, 1.0);\n\
    out_uv = position* 0.5 + 0.5;\n\
}";

static constexpr const char* const fragment_shader_src = "\
#version 300 es\n\
\n\
precision highp float;\n\
precision highp sampler2D;\n\
\n\
uniform float value;\n\
uniform sampler2D source_texture;\n\
\n\
in vec2 out_uv;\n\
\n\
layout(location = 0) out vec4 frag_colour;\n\
\n\
void main() {\n\
    frag_colour = texture(source_texture, out_uv);\n\
    frag_colour.xyz *= value;\n\
}";

gearoenix::gl::shader::Exposure::Exposure(Engine& e) noexcept
    : Shader(e)
{

    set_vertex_shader(vertex_shader_src);
    set_fragment_shader(fragment_shader_src);

    link();
    GX_GL_SHADER_SET_TEXTURE_INDEX_STARTING;
    GX_GL_THIS_GET_UNIFORM(value);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(source_texture);
}

gearoenix::gl::shader::Exposure::~Exposure() noexcept = default;

void gearoenix::gl::shader::Exposure::bind(uint& current_shader) const noexcept
{
    if (shader_program == current_shader)
        return;
    Shader::bind(current_shader);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(source_texture);
}
#endif
