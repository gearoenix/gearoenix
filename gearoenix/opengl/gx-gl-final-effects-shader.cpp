#include "gx-gl-final-effects-shader.hpp"
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
uniform sampler2D position_depth; \n\
uniform sampler2D albedo_metallic; \n\
uniform sampler2D normal_roughness; \n\
uniform sampler2D emission_ambient_occlusion; \n\
\n\
in vec2 out_uv;\n\
\n\
out vec4 frag_colour;\n\
\n\
void main() {\n\
    frag_colour = texture(position_depth, out_uv) +\n\
        texture(albedo_metallic, out_uv) * 0.001 +\n\
        texture(normal_roughness, out_uv) * 0.001 +\n\
        texture(emission_ambient_occlusion, out_uv) * 0.001;\n\
}\n";

gearoenix::gl::FinalEffectsShader::FinalEffectsShader(Engine& e) noexcept
    : Shader(e)
{
    set_vertex_shader(vertex_shader_src);
    set_fragment_shader(fragment_shader_src);
    link();
    GX_GL_SHADER_SET_TEXTURE_INDEX_STARTING
    GX_GL_THIS_GET_UNIFORM_TEXTURE(position_depth)
    GX_GL_THIS_GET_UNIFORM_TEXTURE(albedo_metallic)
    GX_GL_THIS_GET_UNIFORM_TEXTURE(normal_roughness)
    GX_GL_THIS_GET_UNIFORM_TEXTURE(emission_ambient_occlusion)
}

gearoenix::gl::FinalEffectsShader::~FinalEffectsShader() noexcept = default;

void gearoenix::gl::FinalEffectsShader::bind() const noexcept
{
    Shader::bind();
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(position_depth)
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(albedo_metallic)
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(normal_roughness)
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(emission_ambient_occlusion)
}

#endif