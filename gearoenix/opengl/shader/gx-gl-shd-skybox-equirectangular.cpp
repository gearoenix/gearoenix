#include "gx-gl-shd-skybox-equirectangular.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED

namespace {
constexpr auto vertex_shader_body = R"SHADER(
uniform mat4 vp;
uniform vec4 camera_position_box_scale;

layout(location = 0) in vec3 position;

out vec3 out_uv;

void main() {
    out_uv = position;
    gl_Position = vp * vec4((position * camera_position_box_scale.w) + camera_position_box_scale.xyz, 1.0);
}
)SHADER";

constexpr auto fragment_shader_body = R"SHADER(
uniform sampler2D albedo;

in vec3 out_uv;

out vec4 frag_colour;

const vec2 inv_atan = vec2(0.1591, 0.3183);

void main() {
    vec3 v = normalize(out_uv);
    frag_colour = texture(albedo, (vec2(atan(v.y, v.x), asin(v.z)) * inv_atan) + 0.5);
}
)SHADER";
}

gearoenix::gl::shader::SkyboxEquirectangular::SkyboxEquirectangular()
{
    set_vertex_shader(get_common_shader_starter() + vertex_shader_body);
    set_fragment_shader(get_common_shader_starter() + fragment_shader_body);
    link();
    GX_GL_SHADER_SET_TEXTURE_INDEX_STARTING;
    GX_GL_THIS_GET_UNIFORM(vp);
    GX_GL_THIS_GET_UNIFORM(camera_position_box_scale);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(albedo);
}

gearoenix::gl::shader::SkyboxEquirectangular::~SkyboxEquirectangular() = default;

void gearoenix::gl::shader::SkyboxEquirectangular::bind(uint& current_shader) const
{
    if (shader_program == current_shader)
        return;
    Shader::bind(current_shader);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(albedo);
}

#endif