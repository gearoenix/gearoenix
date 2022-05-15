#include "gx-gl-shader-gbuffers-filler.hpp"
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
layout(location = " GX_STRINGIFY(GEAROENIX_GL_GBUFFER_FRAMEBUFFER_ATTACHMENT_INDEX_ALBEDO_METALLIC) ") out vec4 frag_out_albedo_metallic;\n\
layout(location = " GX_STRINGIFY(GEAROENIX_GL_GBUFFER_FRAMEBUFFER_ATTACHMENT_INDEX_POSITION_DEPTH) ") out vec4 frag_out_position_depth;\n\
layout(location = " GX_STRINGIFY(GEAROENIX_GL_GBUFFER_FRAMEBUFFER_ATTACHMENT_INDEX_NORMAL_AO) ") out vec4 frag_out_normal_ao;\n\
layout(location = " GX_STRINGIFY(GEAROENIX_GL_GBUFFER_FRAMEBUFFER_ATTACHMENT_INDEX_EMISSION_ROUGHNESS) ") out vec4 frag_out_emission_roughness;\n\
\n\
void main() {\n\
\n\
    vec2 mtr = texture(metallic_roughness, out_uv).xy * vec2(normal_metallic_factor.w, emission_roughness_factor.w);\n\
\n\
    float ao = texture(occlusion, out_uv).x * alpha_cutoff_occlusion_strength_radiance_lod_coefficient_reserved.y;\n\
\n\
    frag_out_albedo_metallic = vec4(texture(albedo, out_uv).xyz * albedo_factor.xyz, mtr.x) * 0.0001 + vec4(texture(irradiance, normalize(out_pos)).xyz * 0.0001 + textureLod(radiance, normalize(out_pos), 0.0).xyz, 1.0);\n\
\n\
    frag_out_position_depth = vec4(out_pos, gl_FragCoord.z);\n\
\n\
    frag_out_normal_ao = vec4(normalize(mat3(out_tng, out_btg, out_nrm) * ((texture(normal, out_uv).xyz * 2.0 - 1.0) * normal_metallic_factor.xyz)), ao);\n\
\n\
    frag_out_emission_roughness = vec4(texture(emission, out_uv).xyz * emission_roughness_factor.xyz, mtr.y);\n\
\n\
}\n";

gearoenix::gl::ShaderGBuffersFiller::ShaderGBuffersFiller(Engine& e) noexcept
    : Shader(e)
{
    set_vertex_shader(vertex_shader_src);
    set_fragment_shader(fragment_shader_src);
    link();
    GX_GL_SHADER_SET_TEXTURE_INDEX_STARTING
    GX_GL_THIS_GET_UNIFORM(m)
    GX_GL_THIS_GET_UNIFORM(inv_m)
    GX_GL_THIS_GET_UNIFORM(vp)
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
}

gearoenix::gl::ShaderGBuffersFiller::~ShaderGBuffersFiller() noexcept = default;

void gearoenix::gl::ShaderGBuffersFiller::bind() const noexcept
{
    Shader::bind();
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(albedo)
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(normal)
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(emission)
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(metallic_roughness)
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(occlusion)
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(irradiance)
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(radiance)
}

#endif