#include "gx-gl-shd-skybox-equirectangular.hpp"
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
\n\
#define GX_PI 3.141592653589793238\n\
\n\
precision highp float;\n\
precision highp int;\n\
precision highp sampler2D;\n\
precision highp samplerCube;\n\
\n\
uniform sampler2D albedo;\n\
\n\
in vec3 out_uv;\n\
\n\
out vec4 frag_colour;\n\
\n\
const vec2 inv_atan = vec2(0.1591, 0.3183);\n\
\n\
void main() {\n\
    vec3 v = normalize(out_uv);\n\
    frag_colour = texture(albedo, (vec2(atan(v.y, v.x), asin(v.z)) * inv_atan) + 0.5);\n\
}\n";

gearoenix::gl::shader::SkyboxEquirectangular::SkyboxEquirectangular(Engine& e) noexcept
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

gearoenix::gl::shader::SkyboxEquirectangular::~SkyboxEquirectangular() noexcept = default;

void gearoenix::gl::shader::SkyboxEquirectangular::bind(uint& current_shader) const noexcept
{
    if (shader_program == current_shader)
        return;
    Shader::bind(current_shader);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(albedo);
}

#endif