#include "gx-gl-shd-gbuffers-filler.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../core/macro/gx-cr-mcr-stringifier.hpp"
#include "../gx-gl-constants.hpp"
#include "../gx-gl-engine.hpp"

namespace {
constexpr auto vertex_shader_body = R"SHADER(
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 tangent;
layout(location = 3) in vec2 uv;

uniform mat4 m;
uniform mat4 inv_m;
uniform mat4 vp;
uniform vec3 camera_position;

out vec3 out_pos;
out vec3 out_eye;
out vec3 out_nrm;
out vec3 out_tng;
out vec3 out_btg;
out vec2 out_uv;

void main() {
    out_uv = uv;
    vec4 pos = m * vec4(position, 1.0);
    out_pos = pos.xyz;
    out_eye = normalize(pos.xyz - camera_position);
    out_nrm = (inv_m * vec4(normal, 0.0)).xyz;
    out_tng = (inv_m * vec4(tangent.xyz, 0.0)).xyz;
    out_btg = cross(out_nrm, out_tng) * tangent.w;
    out_uv = uv;
    gl_Position = vp * pos;
}
)SHADER";

constexpr auto fragment_shader_body
    = R"SHADER(
uniform vec4 albedo_factor;
uniform vec4 normal_metallic_factor;
uniform vec4 emission_roughness_factor;
uniform vec4 alpha_cutoff_occlusion_strength_radiance_lod_coefficient_reserved;

uniform sampler2D albedo;
uniform sampler2D normal;
uniform sampler2D emission;
uniform sampler2D metallic_roughness;
uniform sampler2D occlusion;

uniform samplerCube irradiance;
uniform samplerCube radiance;

in vec3 out_pos;
in vec3 out_eye;
in vec3 out_nrm;
in vec3 out_tng;
in vec3 out_btg;
in vec2 out_uv;
)SHADER"

      // clang-format off
"layout(location = " GX_STRINGIFY(GEAROENIX_GL_GBUFFERS_FRAMEBUFFER_ATTACHMENT_INDEX_ALBEDO_METALLIC) ") out vec4 frag_out_albedo_metallic;\n"
"layout(location = " GX_STRINGIFY(GEAROENIX_GL_GBUFFERS_FRAMEBUFFER_ATTACHMENT_INDEX_POSITION_DEPTH) ") out vec4 frag_out_position_depth;\n"
"layout(location = " GX_STRINGIFY(GEAROENIX_GL_GBUFFERS_FRAMEBUFFER_ATTACHMENT_INDEX_NORMAL_AO) ") out vec4 frag_out_normal_ao;\n"
"layout(location = " GX_STRINGIFY(GEAROENIX_GL_GBUFFERS_FRAMEBUFFER_ATTACHMENT_INDEX_EMISSION_ROUGHNESS) ") out vec4 frag_out_emission_roughness;\n"
"layout(location = " GX_STRINGIFY(GEAROENIX_GL_GBUFFERS_FRAMEBUFFER_ATTACHMENT_INDEX_IRRADIANCE) ") out vec3 frag_out_irradiance;\n"
"layout(location = " GX_STRINGIFY(GEAROENIX_GL_GBUFFERS_FRAMEBUFFER_ATTACHMENT_INDEX_RADIANCE) ") out vec3 frag_out_radiance;\n"
// clang-fromat on

R"SHADER(

void main() {
    vec2 mtr = texture(metallic_roughness, out_uv).xy * vec2(normal_metallic_factor.w, emission_roughness_factor.w);
    float ao = texture(occlusion, out_uv).x * alpha_cutoff_occlusion_strength_radiance_lod_coefficient_reserved.y;
    frag_out_albedo_metallic = vec4(texture(albedo, out_uv).xyz * albedo_factor.xyz, mtr.x);
    frag_out_position_depth = vec4(out_pos, gl_FragCoord.z);
    frag_out_normal_ao = vec4(normalize(mat3(out_tng, out_btg, out_nrm) * ((texture(normal, out_uv).xyz * 2.0 - 1.0) * normal_metallic_factor.xyz)), ao);
    frag_out_emission_roughness = vec4(texture(emission, out_uv).xyz * emission_roughness_factor.xyz, mtr.y);
    frag_out_irradiance = texture(irradiance, frag_out_normal_ao.xyz).xyz;
    frag_out_radiance = textureLod(radiance, normalize(reflect(out_eye, frag_out_normal_ao.xyz)), mtr.y * alpha_cutoff_occlusion_strength_radiance_lod_coefficient_reserved.z).xyz;
}
)SHADER";
}

gearoenix::gl::shader::GBuffersFiller::GBuffersFiller()
{
    set_vertex_shader(get_common_shader_starter() + vertex_shader_body);
    set_fragment_shader(get_common_shader_starter() + fragment_shader_body);
    link();
    GX_GL_SHADER_SET_TEXTURE_INDEX_STARTING;
    GX_GL_THIS_GET_UNIFORM(m);
    GX_GL_THIS_GET_UNIFORM(inv_m);
    GX_GL_THIS_GET_UNIFORM(vp);
    GX_GL_THIS_GET_UNIFORM(albedo_factor);
    GX_GL_THIS_GET_UNIFORM(camera_position);
    GX_GL_THIS_GET_UNIFORM(normal_metallic_factor);
    GX_GL_THIS_GET_UNIFORM(emission_roughness_factor);
    GX_GL_THIS_GET_UNIFORM(alpha_cutoff_occlusion_strength_radiance_lod_coefficient_reserved);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(albedo);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(normal);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(emission);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(metallic_roughness);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(occlusion);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(irradiance);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(radiance);
}

gearoenix::gl::shader::GBuffersFiller::~GBuffersFiller() = default;

void gearoenix::gl::shader::GBuffersFiller::bind(uint& current_shader) const
{
    if (shader_program == current_shader) {
        return;
    }
    Shader::bind(current_shader);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(albedo);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(normal);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(emission);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(metallic_roughness);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(occlusion);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(irradiance);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(radiance);
}
#endif