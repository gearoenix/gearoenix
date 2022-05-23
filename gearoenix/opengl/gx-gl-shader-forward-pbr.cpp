#include "gx-gl-shader-forward-pbr.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/macro/gx-cr-mcr-stringifier.hpp"
#include "gx-gl-engine.hpp"

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
layout(location = 0) in vec3 position;\n\
layout(location = 1) in vec3 normal;\n\
layout(location = 2) in vec4 tangent;\n\
layout(location = 3) in vec2 uv;\n\
\n\
uniform mat4 m;\n\
uniform mat4 inv_m;\n\
uniform mat4 vp;\n\
\n\
out vec3 out_pos;\n\
out vec3 out_nrm;\n\
out vec3 out_tng;\n\
out vec3 out_btg;\n\
out vec2 out_uv;\n\
\n\
void main() {\n\
    out_uv = uv;\n\
    vec4 pos = m * vec4(position, 1.0);\n\
    out_pos = pos.xyz;\n\
    out_nrm = (inv_m * vec4(normal, 0.0)).xyz;\n\
    out_tng = (inv_m * vec4(tangent.xyz, 0.0)).xyz;\n\
    out_btg = cross(out_nrm, out_tng) * tangent.w;\n\
    out_uv = uv;\n\
    gl_Position = vp * pos;\n\
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
uniform vec4 camera_position_reserved;\n\
uniform vec4 albedo_factor;\n\
uniform vec4 normal_metallic_factor;\n\
uniform vec4 emission_roughness_factor;\n\
uniform vec4 alpha_cutoff_occlusion_strength_radiance_lod_coefficient_reserved;\n\
\n\
uniform sampler2D albedo;\n\
uniform sampler2D normal;\n\
uniform sampler2D emission;\n\
uniform sampler2D metallic_roughness;\n\
uniform sampler2D occlusion;\n\
uniform sampler2D brdflut;\n\
\n\
uniform samplerCube irradiance;\n\
uniform samplerCube radiance;\n\
\n\
in vec3 out_pos;\n\
in vec3 out_nrm;\n\
in vec3 out_tng;\n\
in vec3 out_btg;\n\
in vec2 out_uv;\n\
\n\
out vec3 frag_colour;\n\
\n\
vec3 fresnel_schlick_roughness(const float nv, const vec3 f0, const float roughness) {\n\
    float inv = 1.0 - nv;\n\
    float inv2 = inv * inv;\n\
    float inv4 = inv2 * inv2;\n\
    float inv5 = inv4 * inv;\n\
    return f0 + ((max(vec3(1.0 - roughness), f0) - f0) * inv5);\n\
}\n\
\n\
void main() {\n\
\n\
    vec4 alb = texture(albedo, out_uv) * albedo_factor;\n\
    if(alb.w <= alpha_cutoff_occlusion_strength_radiance_lod_coefficient_reserved.x) discard;\n\
    vec2 mtr = texture(metallic_roughness, out_uv).xy * vec2(normal_metallic_factor.w, emission_roughness_factor.w);\n\
    float ao = texture(occlusion, out_uv).x * alpha_cutoff_occlusion_strength_radiance_lod_coefficient_reserved.y;\n\
    vec3 nrm = normalize(mat3(out_tng, out_btg, out_nrm) * ((texture(normal, out_uv).xyz * 2.0 - 1.0) * normal_metallic_factor.xyz));\n\
    vec3 ems = texture(emission, out_uv).xyz * emission_roughness_factor.xyz;\n\
    vec3 irr = texture(irradiance, nrm).xyz;\n\
    vec3 f0 = mix(vec3(0.04), alb.xyz, mtr.x);\n\
    vec3 f0_inv = vec3(1.0) - f0;\n\
    vec3 eye = normalize(out_pos - camera_position_reserved.xyz);\n\
    vec3 reflection = reflect(eye, nrm);\n\
    vec3 rad = textureLod(radiance, reflection, mtr.y * alpha_cutoff_occlusion_strength_radiance_lod_coefficient_reserved.z).xyz;\n\
    float normal_dot_view = -dot(nrm, eye);\n\
    vec3 fresnel = fresnel_schlick_roughness(normal_dot_view, f0, mtr.y);\n\
    vec3 kd = (vec3(1.0) - fresnel) * (1.0 - mtr.y);\n\
    vec3 ambient = irr * alb.xyz * kd;\n\
    vec2 brdf = texture(brdflut, vec2(normal_dot_view, mtr.y)).rg;\n\
    ambient += rad * ((fresnel * brdf.x) + brdf.y);\n\
    ambient *= ao;\n\
    frag_colour = vec4(ambient + ems, 1.0).xyz;\n\
}\n";

gearoenix::gl::ShaderForwardPbr::ShaderForwardPbr(Engine& e) noexcept
    : Shader(e)
{
    set_vertex_shader(vertex_shader_src);
    set_fragment_shader(fragment_shader_src);
    link();
    GX_GL_SHADER_SET_TEXTURE_INDEX_STARTING
    GX_GL_THIS_GET_UNIFORM(vp)
    GX_GL_THIS_GET_UNIFORM(camera_position_reserved)
    GX_GL_THIS_GET_UNIFORM(m)
    GX_GL_THIS_GET_UNIFORM(inv_m)
    GX_GL_THIS_GET_UNIFORM(albedo_factor)
    GX_GL_THIS_GET_UNIFORM(normal_metallic_factor)
    GX_GL_THIS_GET_UNIFORM(emission_roughness_factor)
    GX_GL_THIS_GET_UNIFORM(alpha_cutoff_occlusion_strength_radiance_lod_coefficient_reserved)
    GX_GL_THIS_GET_UNIFORM_TEXTURE(albedo)
    GX_GL_THIS_GET_UNIFORM_TEXTURE(normal)
    GX_GL_THIS_GET_UNIFORM_TEXTURE(emission)
    GX_GL_THIS_GET_UNIFORM_TEXTURE(metallic_roughness)
    GX_GL_THIS_GET_UNIFORM_TEXTURE(occlusion)
    GX_GL_THIS_GET_UNIFORM_TEXTURE(irradiance)
    GX_GL_THIS_GET_UNIFORM_TEXTURE(radiance)
    GX_GL_THIS_GET_UNIFORM_TEXTURE(brdflut)
    // GX_GL_THIS_GET_UNIFORM_TEXTURE(shadow)
}

gearoenix::gl::ShaderForwardPbr::~ShaderForwardPbr() noexcept = default;

void gearoenix::gl::ShaderForwardPbr::bind() const noexcept
{
    Shader::bind();
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(albedo)
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(normal)
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(emission)
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(metallic_roughness)
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(occlusion)
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(irradiance)
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(radiance)
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(brdflut)
    // GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(shadow)
}

#endif