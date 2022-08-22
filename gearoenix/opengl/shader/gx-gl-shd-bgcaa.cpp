#include "gx-gl-shd-bgcaa.hpp"
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
uniform vec2 screen_space_uv;\n\
\n\
uniform sampler2D low_texture;\n\
uniform sampler2D high_texture;\n\
uniform sampler2D depth_texture;\n\
\n\
in vec2 out_uv;\n\
\n\
out vec4 frag_colour;\n\
\n\
const float bloom_radius = 0.0001;\n\
const float bloom_horizontal = 0.2;\n\
const float exposure = 1.2;\n\
const float gamma = 2.2;\n\
void main() {\n\
    vec3 bloom = vec3(0.0);\n\
    for(vec2 br_uv = out_uv - bloom_radius; br_uv.x < out_uv.x + bloom_radius; br_uv.x += screen_space_uv.x) {\n\
        for(br_uv.y = out_uv.y - bloom_radius; br_uv.y < out_uv.y + bloom_radius; br_uv.y += screen_space_uv.y) { \n\
            bloom += texture(high_texture, br_uv).xyz * smoothstep(0.0, 1.0, 1.0 - length(br_uv - out_uv) / bloom_radius);\n\
        }\n\
    }\n\
    frag_colour = texture(low_texture, out_uv) + texture(high_texture, out_uv) + 0.001 * vec4(bloom, 1.0) + 0.0001 * texture(depth_texture, out_uv);\n\
    frag_colour.xyz = vec3(1.0) - exp(-frag_colour.xyz * exposure);\n\
    frag_colour.xyz = pow(frag_colour.xyz, vec3(1.0 / gamma));\n\
}\n";

gearoenix::gl::shader::BGCAA::BGCAA(Engine& e) noexcept
    : Shader(e)
{
    set_vertex_shader(vertex_shader_src);
    set_fragment_shader(fragment_shader_src);
    link();
    GX_GL_SHADER_SET_TEXTURE_INDEX_STARTING;
    GX_GL_THIS_GET_UNIFORM(screen_space_uv);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(low_texture);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(high_texture);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(depth_texture);
}

gearoenix::gl::shader::BGCAA::~BGCAA() noexcept = default;

void gearoenix::gl::shader::BGCAA::bind() const noexcept
{
    Shader::bind();
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(low_texture);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(high_texture);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(depth_texture);
}

#endif