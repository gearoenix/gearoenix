#include "gx-gl-gbuffers-filler-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "gx-gl-engine.hpp"

gearoenix::gl::GBuffersFillerShader::GBuffersFillerShader(Engine& e) noexcept
    : Shader(e)
{
    GX_GL_SHADER_SRC_DEFAULT_VERTEX_STARTING << "uniform mat4 m;\n"
                                                "uniform mat4 inv_m;\n"
                                                "uniform mat4 vp;\n"
                                                "out vec3 out_pos;\n"
                                                "out vec3 out_nrm;\n"
                                                "out vec3 out_tng;\n"
                                                "out vec3 out_btg;\n"
                                                "out vec2 out_uv;\n"
                                                "void main() {\n"
                                                "    out_uv = uv;\n"
                                                "    vec4 pos = m * vec4(position, 1.0);\n"
                                                "    out_pos = pos.xyz;\n"
                                                "    out_nrm = (inv_m * float4(normal, 0.0)).xyz;\n"
                                                "    out_tng = (inv_m * float4(tangent, 0.0)).xyz;\n"
                                                "    out_btg = cross(out_nrm, out_tng) * tangent.w;\n"
                                                "    out_uv = uv;\n"
                                                "    gl_Position = vp * pos;\n"
                                                "}";
    GX_GL_SHADER_SRC_DEFAULT_FRAGMENT_STARTING << "uniform vec4 albedo_factor;\n"
                                                  "uniform vec4 normal_metallic_factor;\n"
                                                  "uniform vec4 emission_roughness_factor;\n"
                                                  "uniform vec4 camera_position;\n"
                                                  "uniform sampler2D albedo;\n"
                                                  "uniform sampler2D normal_metallic;\n"
                                                  "uniform sampler2D emission_roughness;\n"
                                                  "in vec3 out_pos;\n"
                                                  "in vec3 out_nrm;\n"
                                                  "in vec3 out_tng;\n"
                                                  "in vec3 out_btg;\n"
                                                  "in vec2 out_uv;\n"
                                                  "out vec4 frag_color;\n"
                                                  "void main() {\n"
                                                  "    vec4 frag_color = texture(albedo, out_uv);\n"
                                                  "    frag_color *= albedo_factor;\n"
                                                  "    frag_color += (vec4(out_pos + out_nrm + out_tng + out_btg, 0.0) + camera_position) * 0.0001;\n"
                                                  "}";
    set_vertex_shader(vertex_shader_code.str());
    set_fragment_shader(fragment_shader_code.str());
    link();
    GX_GL_SHADER_SET_TEXTURE_INDEX_STARTING
    GX_GL_THIS_GET_UNIFORM(m)
    GX_GL_THIS_GET_UNIFORM(inv_m)
    GX_GL_THIS_GET_UNIFORM(vp)
    GX_GL_THIS_GET_UNIFORM(albedo_factor)
    GX_GL_THIS_GET_UNIFORM(normal_metallic_factor)
    GX_GL_THIS_GET_UNIFORM(emission_roughness_factor)
    GX_GL_THIS_GET_UNIFORM(camera_position)
    GX_GL_THIS_GET_UNIFORM_TEXTURE(albedo)
    GX_GL_THIS_GET_UNIFORM_TEXTURE(normal_metallic)
    GX_GL_THIS_GET_UNIFORM_TEXTURE(emission_roughness)
}

gearoenix::gl::GBuffersFillerShader::~GBuffersFillerShader() noexcept = default;

void gearoenix::gl::GBuffersFillerShader::bind() const noexcept
{
    Shader::bind();
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(albedo)
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(normal_metallic)
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(emission_roughness)
}

#endif