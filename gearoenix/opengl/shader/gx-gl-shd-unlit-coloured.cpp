#include "gx-gl-shd-unlit-coloured.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED

static constexpr const char* const vertex_shader_src = "\
#version 300 es\n\
\n\
precision highp float;\n\
\n\
layout(location = 0) in vec3 position;\n\
\n\
uniform mat4 mvp;\n\
\n\
void main() {\n\
    gl_Position = mvp * vec4(position, 1.0);\n\
}\n";

static constexpr const char* const fragment_shader_src = "\
#version 300 es\n\
\n\
precision highp float;\n\
\n\
uniform vec3 colour;\n\
\n\
out vec3 frag_colour;\n\
\n\
void main() {\n\
    frag_colour = colour;\n\
}\n";

gearoenix::gl::shader::UnlitColoured::UnlitColoured(Engine& e) noexcept
    : Shader(e)
{
    set_vertex_shader(vertex_shader_src);
    set_fragment_shader(fragment_shader_src);
    link();
    GX_GL_THIS_GET_UNIFORM(mvp);
    GX_GL_THIS_GET_UNIFORM(colour);
}

gearoenix::gl::shader::UnlitColoured::~UnlitColoured() noexcept = default;

void gearoenix::gl::shader::UnlitColoured::bind(uint& current_shader) const noexcept
{
    if (shader_program == current_shader)
        return;
    Shader::bind(current_shader);
}

#endif