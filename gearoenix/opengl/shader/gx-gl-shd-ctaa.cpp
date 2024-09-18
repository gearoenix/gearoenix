#include "gx-gl-shd-ctaa.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include <boost/mp11/algorithm.hpp>

static constexpr const char* const vertex_shader_src = "\
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
}\n";

gearoenix::gl::shader::ColourTuningAntiAliasing::ColourTuningAntiAliasing(Engine& e, const std::size_t colour_tuning_index)
    : Shader(e)
{
    const bool is_gamma_correction_index = colour_tuning_index == boost::mp11::mp_find<render::camera::ColourTuning, render::camera::GammaCorrection>::value;
    const bool is_colour_scale_index = colour_tuning_index == boost::mp11::mp_find<render::camera::ColourTuning, render::camera::Multiply>::value;

    set_vertex_shader(vertex_shader_src);
    std::stringstream fs;
    fs << "#version 300 es\n";
    fs << "\n";
    fs << "#define GX_PI 3.141592653589793238\n";
    fs << "\n";
    fs << "precision highp float;\n";
    fs << "precision highp sampler2D;\n";
    fs << "\n";
    fs << "uniform vec2 screen_space_uv;\n";
    if (is_gamma_correction_index) {
        fs << "uniform vec3 gamma_exponent;\n";
    } else if (is_colour_scale_index) {
        fs << "uniform vec3 colour_scale;\n";
    }
    fs << "\n";
    fs << "uniform sampler2D source_texture;\n";
    fs << "uniform sampler2D depth_texture;\n";
    fs << "\n";
    fs << "in vec2 out_uv;\n";
    fs << "\n";
    fs << "out vec4 frag_colour;\n";
    fs << "\n";
    fs << "vec3 uncharted2_tonemap_partial(vec3 x) {\n";
    fs << "    const float a = 0.15f;\n";
    fs << "    const float b = 0.50f;\n";
    fs << "    const float c = 0.10f;\n";
    fs << "    const float d = 0.20f;\n";
    fs << "    const float e = 0.02f;\n";
    fs << "    const float f = 0.30f;\n";
    fs << "    return ((x * (a * x + c * b) + d * e) / (x * (a * x + b) + d * f)) - e / f;\n";
    fs << "}\n";
    fs << "\n";
    fs << "vec3 uncharted2_filmic(vec3 v) {\n";
    //    fs << "    const float exposure_bias = 2.0f;\n";
    //    fs << "    vec3 curr = uncharted2_tonemap_partial(v * exposure_bias);\n";
    //    fs << "    const vec3 W = vec3(11.2f);\n";
    //    fs << "    vec3 white_scale = vec3(1.0f) / uncharted2_tonemap_partial(W);\n";
    //    fs << "    return curr * white_scale;\n";
    fs << "    return uncharted2_tonemap_partial(v * 2.0f) * 1.37906f;\n";
    fs << "}\n";
    fs << "\n";
    fs << "void main() {\n";
    fs << "    frag_colour = texture(source_texture, out_uv);\n";

    fs << "    frag_colour.x += 0.0001 * (texture(depth_texture, out_uv).x + screen_space_uv.x);\n"; // Temporary, only for keeping the uniforms alive

    if (is_gamma_correction_index) {
        // The code for ACES tone mapping is based on the code written by Stephen Hill (@self_shadow), who deserves all
        // credit for coming up with this fit and implementing it. Buy him a beer next time you see him. :)
        // sRGB => XYZ => D65_2_D60 => AP1 => RRT_SAT
        // fs << "    frag_colour.xyz = mat3(\n";
        // fs << "        0.59719, 0.35458, 0.04823,\n";
        // fs << "        0.07600, 0.90834, 0.01566,\n";
        // fs << "        0.02840, 0.13383, 0.83777) * frag_colour.xyz;\n";
        // RRT And ODT Fit
        // fs << "    {\n";
        // fs << "        vec3 a = frag_colour.xyz * (frag_colour.xyz + 0.0245786f) - 0.000090537f;\n";
        // fs << "        vec3 b = frag_colour.xyz * (0.983729f * frag_colour.xyz + 0.4329510f) + 0.238081f;\n";
        // fs << "        frag_colour.xyz = a / b;\n";
        // fs << "    }\n";
        // ODT_SAT => XYZ => D60_2_D65 => sRGB
        // fs << "    frag_colour.xyz = mat3(\n";
        // fs << "        1.60475, -0.53108, -0.07367,\n";
        // fs << "        -0.10208,  1.10813, -0.00605,\n";
        // fs << "        -0.00327, -0.07276,  1.07602) * frag_colour.xyz;\n";

        // fs << "    float c_max = max(frag_colour.x, max(frag_colour.y, frag_colour.z));\n";
        // fs << "    frag_colour.xyz = frag_colour.xyz * (2.0 / (c_max * c_max + 1.0f));\n";

        fs << "    frag_colour.xyz = frag_colour.xyz / (frag_colour.xyz + 1.0f);\n";

        // fs << "    frag_colour.xyz = uncharted2_filmic(frag_colour.xyz);\n";
        // fs << "    frag_colour.xyz *= (clamp(0.6 - frag_colour.xyz, 0.0, 1.0) * 1.3 + 1.0) /* 0.00001 + 1.0*/;\n";
        fs << "    frag_colour.xyz += pow(frag_colour.xyz, gamma_exponent);\n";
    } else if (is_colour_scale_index) {
        fs << "    frag_colour.xyz *= colour_scale;\n";
    }
    fs << "}\n";
    set_fragment_shader(fs.str());
    link();
    GX_GL_SHADER_SET_TEXTURE_INDEX_STARTING;
    GX_GL_THIS_GET_UNIFORM(screen_space_uv);
    if (is_gamma_correction_index) {
        GX_GL_THIS_GET_UNIFORM(gamma_exponent);
    } else if (is_colour_scale_index) {
        GX_GL_THIS_GET_UNIFORM(colour_scale);
    }
    GX_GL_THIS_GET_UNIFORM_TEXTURE(source_texture);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(depth_texture);
}

gearoenix::gl::shader::ColourTuningAntiAliasing::~ColourTuningAntiAliasing() = default;

void gearoenix::gl::shader::ColourTuningAntiAliasing::bind(uint& current_shader) const
{
    if (shader_program == current_shader)
        return;
    Shader::bind(current_shader);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(source_texture);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(depth_texture);
}

void gearoenix::gl::shader::ColourTuningAntiAliasing::set(const render::camera::ColourTuning& colour_tuning)
{
    switch (colour_tuning.index()) {
    case boost::mp11::mp_find<render::camera::ColourTuning, render::camera::GammaCorrection>::value:
        set_gamma_exponent_data(reinterpret_cast<const float*>(&std::get<render::camera::GammaCorrection>(colour_tuning)));
        break;
    case boost::mp11::mp_find<render::camera::ColourTuning, render::camera::Multiply>::value:
        set_colour_scale_data(reinterpret_cast<const float*>(&std::get<render::camera::Multiply>(colour_tuning).scale));
        break;
    }
}

gearoenix::gl::shader::ColourTuningAntiAliasingCombination::ColourTuningAntiAliasingCombination(Engine& e)
    : e(e)
{
}

#endif
