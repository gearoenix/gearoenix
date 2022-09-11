#include "gx-gl-shd-gcaa.hpp"
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
layout(location = 0) in vec2 position;\n\
\n\
out vec2 out_uv;\n\
\n\
void main() {\n\
    gl_Position = vec4(position, 0.0, 1.0);\n\
    out_uv = position* 0.5 + 0.5;\n\
}\n";

static constexpr const char* const fragment_shader_src = "\
#version 300 es\n\
\n\
#define GX_PI 3.141592653589793238\n\
\n\
precision highp float;\n\
precision highp int;\n\
precision highp sampler2D;\n\
precision highp samplerCube;\n\
\n\
uniform vec4 screen_space_uv_exposure_gamma;\n\
\n\
uniform sampler2D low_texture;\n\
uniform sampler2D high_texture;\n\
uniform sampler2D depth_texture;\n\
\n\
in vec2 out_uv;\n\
\n\
out vec4 frag_colour;\n\
\n\
void main() {\n\
    frag_colour.xyz = texture(low_texture, out_uv).xyz + texture(high_texture, out_uv).xyz;\n\
    frag_colour.xyz = vec3(1.0) - exp(-frag_colour.xyz * screen_space_uv_exposure_gamma.z) + (0.0001 * texture(depth_texture, out_uv).xyz);\n\
    frag_colour.xyz = pow(frag_colour.xyz, vec3(1.0 / screen_space_uv_exposure_gamma.w));\n\
    frag_colour.w = 1.0;\n\
}\n";

gearoenix::gl::shader::GamaCorrectionColourTuningAntiAliasing::GamaCorrectionColourTuningAntiAliasing(Engine& e) noexcept
    : Shader(e)
{
    set_vertex_shader(vertex_shader_src);
    set_fragment_shader(fragment_shader_src);
    link();
    GX_GL_SHADER_SET_TEXTURE_INDEX_STARTING;
    GX_GL_THIS_GET_UNIFORM(screen_space_uv_exposure_gamma);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(low_texture);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(high_texture);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(depth_texture);
}

gearoenix::gl::shader::GamaCorrectionColourTuningAntiAliasing::~GamaCorrectionColourTuningAntiAliasing() noexcept = default;

void gearoenix::gl::shader::GamaCorrectionColourTuningAntiAliasing::bind() const noexcept
{
    Shader::bind();
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(low_texture);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(high_texture);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(depth_texture);
}

#endif