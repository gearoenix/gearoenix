#include "gx-gl-shd-shadow-caster.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../core/macro/gx-cr-mcr-stringifier.hpp"
#include "../gx-gl-engine.hpp"

static constexpr const char* const vertex_shader_src = "\
#version 300 es\n\
\n\
precision highp float;\n\
\n\
layout(location = 0) in vec3 position;\n\
layout(location = 3) in vec2 uv;\n\
\n\
uniform mat4 mvp;\n\
\n\
out vec2 out_uv;\n\
\n\
void main() {\n\
    out_uv = uv;\n\
    gl_Position = mvp * vec4(position, 1.0);\n\
}\n";

static constexpr const char* const fragment_shader_src = "\
#version 300 es\n\
\n\
precision highp float;\n\
\n\
uniform vec2 alpha_factor_alpha_cutoff;\n\
\n\
uniform sampler2D albedo;\n\
\n\
in vec2 out_uv;\n\
\n\
out vec3 frag_colour;\n\
\n\
void main() {\n\
\n\
    float alpha = texture(albedo, out_uv).w * alpha_factor_alpha_cutoff.x;\n\
    if(alpha <= alpha_factor_alpha_cutoff.y) discard;\n\
    frag_colour = vec3(1.0);\n\
}\n";

gearoenix::gl::shader::ShadowCaster::ShadowCaster(Engine& e) noexcept
    : Shader(e)
{
    set_vertex_shader(vertex_shader_src);
    set_fragment_shader(fragment_shader_src);
    link();
    GX_GL_SHADER_SET_TEXTURE_INDEX_STARTING;
    GX_GL_THIS_GET_UNIFORM(mvp);
    GX_GL_THIS_GET_UNIFORM(alpha_factor_alpha_cutoff);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(albedo);
}

gearoenix::gl::shader::ShadowCaster::~ShadowCaster() noexcept = default;

void gearoenix::gl::shader::ShadowCaster::bind() const noexcept
{
    Shader::bind();
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(albedo);
}

#endif