#include "gx-gl-shd-skybox-cube.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED

static constexpr const char* const vertex_shader_src = "\
#version 300 es\n\
\n\
#define GX_PI 3.141592653589793238\n\
\n\
precision highp float;\n\
precision highp int;\n\
precision highp sampler2D;\n\
precision highp samplerCube;\n\
\n\
uniform mat4 vp;\n\
uniform vec4 camera_position_box_scale;\n\
\n\
layout(location = 0) in vec3 position;\n\
\n\
out vec3 out_uv;\n\
\n\
void main() {\n\
    out_uv = position;\n\
    gl_Position = vp * vec4((position * camera_position_box_scale.w) + camera_position_box_scale.xyz, 1.0);\n\
}\n";

static constexpr const char* const fragment_shader_src = "\
#version 300 es\n\
\n\
#extension GL_OES_texture_float: enable\n\
#extension GL_OES_texture_float_linear: enable\n\
#extension OES_texture_float: enable\n\
#extension OES_texture_float_linear: enable\n\
\n\
#define GX_PI 3.141592653589793238\n\
\n\
precision highp float;\n\
precision highp int;\n\
precision highp sampler2D;\n\
precision highp samplerCube;\n\
\n\
uniform samplerCube albedo;\n\
\n\
in vec3 out_uv;\n\
\n\
out vec4 frag_colour;\n\
\n\
void main() {\n\
    frag_colour = texture(albedo, normalize(out_uv));\n\
}\n";

gearoenix::gl::shader::SkyboxCube::SkyboxCube(Engine& e)
    : Shader(e)
{
    set_vertex_shader(vertex_shader_src);
    set_fragment_shader(fragment_shader_src);
    link();
    GX_GL_SHADER_SET_TEXTURE_INDEX_STARTING;
    GX_GL_THIS_GET_UNIFORM(vp);
    GX_GL_THIS_GET_UNIFORM(camera_position_box_scale);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(albedo);
}

gearoenix::gl::shader::SkyboxCube::~SkyboxCube() = default;

void gearoenix::gl::shader::SkyboxCube::bind(uint& current_shader) const
{
    if (shader_program == current_shader)
        return;
    Shader::bind(current_shader);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(albedo);
}

#endif