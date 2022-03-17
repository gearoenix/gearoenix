#include "gx-gl-gbuffers-filler-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "gx-gl-engine.hpp"

static constexpr const char* const vertex_shader_src = "#version 320 es\n"
                                                       "#define GX_PI 3.141592653589793238\n"
                                                       "\n"
                                                       "precision highp float;\n"
                                                       "precision highp int;\n"
                                                       "precision highp sampler2D;\n"
                                                       "precision highp samplerCube;\n"
                                                       "\n"
                                                       "layout(location = 0) in vec3 position;\n"
                                                       "layout(location = 1) in vec3 normal;\n"
                                                       "layout(location = 2) in vec4 tangent;\n"
                                                       "layout(location = 3) in vec2 uv;\n"
                                                       "\n"
                                                       "uniform mat4 m;\n"
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
                                                       "    out_nrm = (inv_m * vec4(normal, 0.0)).xyz;\n"
                                                       "    out_tng = (inv_m * vec4(tangent.xyz, 0.0)).xyz;\n"
                                                       "    out_btg = cross(out_nrm, out_tng) * tangent.w;\n"
                                                       "    out_uv = uv;\n"
                                                       "    gl_Position = vp * pos;\n"
                                                       "}";

static constexpr const char* const fragment_shader_src = "#version 320 es\n"
                                                         //"#extension GL_OES_texture_float: require\n"
                                                         //"#extension GL_OES_texture_float_linear: require\n"
                                                         "#define GX_PI 3.141592653589793238\n"
                                                         "\n"
                                                         "precision highp float;\n"
                                                         "precision highp int;\n"
                                                         "precision highp sampler2D;\n"
                                                         "precision highp samplerCube;\n"
                                                         "\n"
                                                         "uniform vec4 albedo_factor;\n"
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
                                                         "out vec4 frag_colour;\n"
                                                         "void main() {\n"
                                                         "    frag_colour = texture(albedo, out_uv) * albedo_factor;\n"
                                                         "    vec4 temp = vec4(out_pos + out_nrm + out_tng + out_btg, 0.0);\n"
                                                         "    temp += camera_position;\n"
                                                         "    temp += texture(normal_metallic, out_uv) * normal_metallic_factor;\n"
                                                         "    temp += texture(emission_roughness, out_uv) * emission_roughness_factor;\n"
                                                         "    frag_colour += temp * 0.0000001;\n"
                                                         "}";

gearoenix::gl::GBuffersFillerShader::GBuffersFillerShader(Engine& e) noexcept
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