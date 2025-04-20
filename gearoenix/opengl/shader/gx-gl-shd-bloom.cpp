#include "gx-gl-shd-bloom.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED

namespace {
constexpr auto vertex_shader_src = "\
#version 300 es\n\
\n\
precision highp float;\n\
\n\
layout(location = 0) in vec2 position;\n\
\n\
out vec2 out_uv;\n\
\n\
void main() {\n\
    gl_Position = vec4(position, 0.0, 1.0);\n\
    out_uv = position * 0.5 + 0.5;\n\
}";
}

gearoenix::gl::shader::BloomPrefilter::BloomPrefilter()
{
    // Vertex Shader -----------------------------------------------------------------------------------
    set_vertex_shader(vertex_shader_src);

    // Fragment Shader ---------------------------------------------------------------------------------
    std::stringstream fs;
    fs << "#version 300 es\n";
    fs << "\n";
    fs << "precision highp float;\n";
    fs << "precision highp int;\n";
    fs << "precision highp sampler2D;\n";
    fs << "\n";
    fs << "uniform vec2 texel_size;\n";
    fs << "uniform vec4 scatter_clamp_max_threshold_threshold_knee;\n";
    fs << "uniform sampler2D source_texture;\n";
    fs << "\n";
    fs << "in vec2 out_uv;\n";
    fs << "\n";
    fs << "layout(location = 0) out vec4 frag_colour;\n";
    fs << "\n";
    fs << "void main() {\n";
    fs << "    vec4 a = textureLod(source_texture, out_uv + texel_size * vec2(-1.0, -1.0), 0.0);\n";
    fs << "    vec4 b = textureLod(source_texture, out_uv + texel_size * vec2(0.0, -1.0), 0.0);\n";
    fs << "    vec4 c = textureLod(source_texture, out_uv + texel_size * vec2(1.0, -1.0), 0.0);\n";
    fs << "    vec4 d = textureLod(source_texture, out_uv + texel_size * vec2(-0.5, -0.5), 0.0);\n";
    fs << "    vec4 e = textureLod(source_texture, out_uv + texel_size * vec2(0.5, -0.5), 0.0);\n"; // TODO I have to check to see if it needs to be doubled, and remove the mip map in that case
    fs << "    vec4 f = textureLod(source_texture, out_uv + texel_size * vec2(-1.0, 0.0), 0.0);\n";
    fs << "    vec4 g = textureLod(source_texture, out_uv, 0.0);\n";
    fs << "    vec4 h = textureLod(source_texture, out_uv + texel_size * vec2(1.0, 0.0), 0.0);\n";
    fs << "    vec4 i = textureLod(source_texture, out_uv + texel_size * vec2(-0.5, 0.5), 0.0);\n";
    fs << "    vec4 j = textureLod(source_texture, out_uv + texel_size * vec2(0.5, 0.5), 0.0);\n";
    fs << "    vec4 k = textureLod(source_texture, out_uv + texel_size * vec2(-1.0, 1.0), 0.0);\n";
    fs << "    vec4 l = textureLod(source_texture, out_uv + texel_size * vec2(0.0, 1.0), 0.0);\n";
    fs << "    vec4 m = textureLod(source_texture, out_uv + texel_size * vec2(1.0, 1.0), 0.0);\n";
    fs << "    const vec2 div = (1.0 / 4.0) * vec2(0.5, 0.125);\n";
    fs << "    frag_colour = (d + e + i + j) * div.x;\n";
    fs << "    frag_colour += (a + b + g + f) * div.y;\n";
    fs << "    frag_colour += (b + c + h + g) * div.y;\n";
    fs << "    frag_colour += (f + g + l + k) * div.y;\n";
    fs << "    frag_colour += (g + h + m + l) * div.y;\n";
    fs << "    frag_colour = min(vec4(scatter_clamp_max_threshold_threshold_knee.y), frag_colour);\n";
    fs << "    float brightness = max(frag_colour.x, max(frag_colour.y, frag_colour.z));\n";
    fs << "    float softness = clamp(brightness - scatter_clamp_max_threshold_threshold_knee.z + scatter_clamp_max_threshold_threshold_knee.w, 0.0, 2.0 * scatter_clamp_max_threshold_threshold_knee.w);\n";
    fs << "    softness = (softness * softness) / (4.0 * scatter_clamp_max_threshold_threshold_knee.w + 1e-4);\n";
    fs << "    float multiplier = max(brightness - scatter_clamp_max_threshold_threshold_knee.z, softness) / max(brightness, 1e-4);\n";
    fs << "    frag_colour *= multiplier;\n";
    fs << "    frag_colour = max(frag_colour, vec4(0.0));\n";
    fs << "}\n";
    set_fragment_shader(fs.str());

    link();
    GX_GL_SHADER_SET_TEXTURE_INDEX_STARTING;
    GX_GL_THIS_GET_UNIFORM(texel_size);
    GX_GL_THIS_GET_UNIFORM(scatter_clamp_max_threshold_threshold_knee);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(source_texture);
}

gearoenix::gl::shader::BloomPrefilter::~BloomPrefilter() = default;

void gearoenix::gl::shader::BloomPrefilter::bind(uint& current_shader) const
{
    if (shader_program == current_shader)
        return;
    Shader::bind(current_shader);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(source_texture);
}

gearoenix::gl::shader::BloomHorizontal::BloomHorizontal()
{
    set_vertex_shader(vertex_shader_src);

    // Fragment Shader ---------------------------------------------------------------------------------
    std::stringstream fs;
    fs << "#version 300 es\n";
    fs << "\n";
    fs << "#define gx_pi 3.141592653589793238\n";
    fs << "\n";
    fs << "precision highp float;\n";
    fs << "precision highp int;\n";
    fs << "precision highp sampler2D;\n";
    fs << "precision highp samplerCube;\n";
    fs << "\n";
    fs << "uniform vec3 texel_size_mip_index;\n";
    fs << "\n";
    fs << "uniform sampler2D source_texture;\n";
    fs << "\n";
    fs << "in vec2 out_uv;\n";
    fs << "\n";
    fs << "out vec4 frag_colour;\n";
    fs << "\n";
    fs << "void main() {\n";
    fs << "    frag_colour = textureLod(source_texture, out_uv - vec2(texel_size_mip_index.x * 4.0, 0.0), texel_size_mip_index.z) * 0.01621622;\n";
    fs << "    frag_colour += textureLod(source_texture, out_uv - vec2(texel_size_mip_index.x * 3.0, 0.0), texel_size_mip_index.z) * 0.05405405;\n";
    fs << "    frag_colour += textureLod(source_texture, out_uv - vec2(texel_size_mip_index.x * 2.0, 0.0), texel_size_mip_index.z) * 0.12162162;\n";
    fs << "    frag_colour += textureLod(source_texture, out_uv - vec2(texel_size_mip_index.x * 1.0, 0.0), texel_size_mip_index.z) * 0.19459459;\n";
    fs << "    frag_colour += textureLod(source_texture, out_uv, texel_size_mip_index.z) * 0.22702703;\n";
    fs << "    frag_colour += textureLod(source_texture, out_uv + vec2(texel_size_mip_index.x * 1.0, 0.0), texel_size_mip_index.z) * 0.19459459;\n";
    fs << "    frag_colour += textureLod(source_texture, out_uv + vec2(texel_size_mip_index.x * 2.0, 0.0), texel_size_mip_index.z) * 0.12162162;\n";
    fs << "    frag_colour += textureLod(source_texture, out_uv + vec2(texel_size_mip_index.x * 3.0, 0.0), texel_size_mip_index.z) * 0.05405405;\n";
    fs << "    frag_colour += textureLod(source_texture, out_uv + vec2(texel_size_mip_index.x * 4.0, 0.0), texel_size_mip_index.z) * 0.01621622;\n";
    fs << "}\n";
    set_fragment_shader(fs.str());

    link();
    GX_GL_SHADER_SET_TEXTURE_INDEX_STARTING;
    GX_GL_THIS_GET_UNIFORM(texel_size_mip_index);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(source_texture);
}

gearoenix::gl::shader::BloomHorizontal::~BloomHorizontal() = default;

void gearoenix::gl::shader::BloomHorizontal::bind(uint& current_shader) const
{
    if (shader_program == current_shader)
        return;
    Shader::bind(current_shader);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(source_texture);
}

gearoenix::gl::shader::BloomVertical::BloomVertical()
{
    set_vertex_shader(vertex_shader_src);

    // Fragment Shader ---------------------------------------------------------------------------------
    std::stringstream fs;
    fs << "#version 300 es\n";
    fs << "\n";
    fs << "#define gx_pi 3.141592653589793238\n";
    fs << "\n";
    fs << "precision highp float;\n";
    fs << "precision highp int;\n";
    fs << "precision highp sampler2D;\n";
    fs << "precision highp samplerCube;\n";
    fs << "\n";
    fs << "uniform vec3 texel_size_mip_index;\n";
    fs << "\n";
    fs << "uniform sampler2D source_texture;\n";
    fs << "\n";
    fs << "in vec2 out_uv;\n";
    fs << "\n";
    fs << "out vec4 frag_colour;\n";
    fs << "\n";
    fs << "void main() {\n";
    fs << "    frag_colour = textureLod(source_texture, out_uv - vec2(0.0, texel_size_mip_index.y * 3.23076923), texel_size_mip_index.z) * 0.07027027;\n";
    fs << "    frag_colour += textureLod(source_texture, out_uv - vec2(0.0, texel_size_mip_index.y * 1.38461538), texel_size_mip_index.z) * 0.31621622;\n";
    fs << "    frag_colour += textureLod(source_texture, out_uv, texel_size_mip_index.z) * 0.22702703;\n";
    fs << "    frag_colour += textureLod(source_texture, out_uv + vec2(0.0, texel_size_mip_index.y * 1.38461538), texel_size_mip_index.z) * 0.31621622;\n";
    fs << "    frag_colour += textureLod(source_texture, out_uv + vec2(0.0, texel_size_mip_index.y * 3.23076923), texel_size_mip_index.z) * 0.07027027;\n";
    fs << "}\n";
    set_fragment_shader(fs.str());

    link();
    GX_GL_SHADER_SET_TEXTURE_INDEX_STARTING;
    GX_GL_THIS_GET_UNIFORM(texel_size_mip_index);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(source_texture);
}

gearoenix::gl::shader::BloomVertical::~BloomVertical() = default;

void gearoenix::gl::shader::BloomVertical::bind(uint& current_shader) const
{
    if (shader_program == current_shader)
        return;
    Shader::bind(current_shader);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(source_texture);
}

gearoenix::gl::shader::BloomUpsampler::BloomUpsampler()
{
    set_vertex_shader(vertex_shader_src);

    // Fragment Shader ---------------------------------------------------------------------------------
    std::stringstream fs;
    fs << "#version 300 es\n";
    fs << "\n";
    fs << "#define gx_pi 3.141592653589793238\n";
    fs << "\n";
    fs << "precision highp float;\n";
    fs << "precision highp int;\n";
    fs << "precision highp sampler2D;\n";
    fs << "precision highp samplerCube;\n";
    fs << "\n";
    fs << "uniform vec2 scatter_src_mip_index;\n";
    fs << "\n";
    fs << "uniform sampler2D source_texture;\n";
    fs << "uniform sampler2D low_texture;\n";
    fs << "\n";
    fs << "in vec2 out_uv;\n";
    fs << "\n";
    fs << "out vec4 frag_colour;\n";
    fs << "\n";
    fs << "void main() {\n";
    fs << "    vec4 src = textureLod(source_texture, out_uv, scatter_src_mip_index.y);\n";
    fs << "    vec4 low = textureLod(low_texture, out_uv, scatter_src_mip_index.y + 1.0f);\n";
    fs << "    frag_colour = src + low * scatter_src_mip_index.x;\n";
    fs << "    frag_colour.w = max(frag_colour.w, src.w);\n";
    fs << "}\n";
    set_fragment_shader(fs.str());

    link();
    GX_GL_SHADER_SET_TEXTURE_INDEX_STARTING;
    GX_GL_THIS_GET_UNIFORM(scatter_src_mip_index);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(source_texture);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(low_texture);
}

gearoenix::gl::shader::BloomUpsampler::~BloomUpsampler() = default;

void gearoenix::gl::shader::BloomUpsampler::bind(uint& current_shader) const
{
    if (shader_program == current_shader) {
        return;
    }
    Shader::bind(current_shader);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(source_texture);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(low_texture);
}

#endif