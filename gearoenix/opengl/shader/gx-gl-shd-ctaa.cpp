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

gearoenix::gl::shader::ColourTuningAntiAliasing::ColourTuningAntiAliasing(Engine& e, const std::size_t colour_tuning_index) noexcept
    : Shader(e)
{
    const bool is_hdr_tune_mapping_gamma_correction_index = colour_tuning_index == boost::mp11::mp_find<render::camera::ColourTuning, render::camera::HdrTuneMappingGammaCorrection>::value;
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
    if (is_hdr_tune_mapping_gamma_correction_index) {
        fs << "uniform vec2 exposure_gamma;\n";
    } else if (is_colour_scale_index) {
        fs << "uniform vec3 colour_scale;\n";
    }
    fs << "\n";
    fs << "uniform sampler2D low_texture;\n";
    fs << "uniform sampler2D high_texture;\n";
    fs << "uniform sampler2D depth_texture;\n";
    fs << "\n";
    fs << "in vec2 out_uv;\n";
    fs << "\n";
    fs << "out vec4 frag_colour;\n";
    fs << "\n";
    fs << "void main() {\n";
    fs << "    frag_colour.xyz = texture(low_texture, out_uv).xyz + texture(high_texture, out_uv).xyz;\n";
    fs << "    frag_colour.xyz += 0.0001 * (texture(depth_texture, out_uv).xyz + screen_space_uv.xyx);\n";
    if (is_hdr_tune_mapping_gamma_correction_index) {
        fs << "    frag_colour.xyz = vec3(1.0) - exp(-frag_colour.xyz * exposure_gamma.x);\n";
        fs << "    frag_colour.xyz = pow(frag_colour.xyz, vec3(1.0 / exposure_gamma.y));\n";
    } else if (is_colour_scale_index) {
        fs << "    frag_colour.xyz *= colour_scale;\n";
    }
    fs << "    frag_colour.w = 1.0;\n";
    fs << "}\n";
    set_fragment_shader(fs.str());
    link();
    GX_GL_SHADER_SET_TEXTURE_INDEX_STARTING;
    GX_GL_THIS_GET_UNIFORM(screen_space_uv);
    if (is_hdr_tune_mapping_gamma_correction_index) {
        GX_GL_THIS_GET_UNIFORM(exposure_gamma);
    } else if (is_colour_scale_index) {
        GX_GL_THIS_GET_UNIFORM(colour_scale);
    }
    GX_GL_THIS_GET_UNIFORM_TEXTURE(low_texture);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(high_texture);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(depth_texture);
}

gearoenix::gl::shader::ColourTuningAntiAliasing::~ColourTuningAntiAliasing() noexcept = default;

void gearoenix::gl::shader::ColourTuningAntiAliasing::bind(uint& current_shader) const noexcept
{
    if (shader_program == current_shader)
        return;
    Shader::bind(current_shader);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(low_texture);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(high_texture);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(depth_texture);
}

void gearoenix::gl::shader::ColourTuningAntiAliasing::set(const render::camera::ColourTuning& colour_tuning) noexcept
{
    switch (colour_tuning.index()) {
    case boost::mp11::mp_find<render::camera::ColourTuning, render::camera::HdrTuneMappingGammaCorrection>::value:
        set_exposure_gamma_data(reinterpret_cast<const float*>(&std::get<render::camera::HdrTuneMappingGammaCorrection>(colour_tuning)));
        break;
    case boost::mp11::mp_find<render::camera::ColourTuning, render::camera::Multiply>::value:
        set_colour_scale_data(reinterpret_cast<const float*>(&std::get<render::camera::Multiply>(colour_tuning).scale));
        break;
    }
}

gearoenix::gl::shader::ColourTuningAntiAliasingCombination::ColourTuningAntiAliasingCombination(Engine& e) noexcept
    : e(e)
{
}

#endif
