#include "gx-gl-shd-bloom.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../core/macro/gx-cr-mcr-counter.hpp"

gearoenix::gl::shader::Bloom::Bloom(Engine& e, const bool is_horizontal) noexcept
    : Shader(e)
{
    constexpr double gaussian_weights[] {
        0.19967562749792111542745942642795853316783905029296875,
        0.1762131227885508633068667450061184354126453399658203125,
        0.12110939007484812501846960230977856554090976715087890625,
        0.06482518513852682240372615751766716130077838897705078125,
        0.0270231576028795265898008182148259948007762432098388671875,
        0.00877313479158838525895358628758913255296647548675537109375,
        0.002218195854645765678625490835429445724003016948699951171875,
    };
    constexpr std::size_t gaussian_weights_count = GX_COUNT_OF(gaussian_weights);

    // Vertex Shader -----------------------------------------------------------------------------------
    std::stringstream vs;
    vs << "#version 300 es\n";
    vs << "\n";
    vs << "precision highp float;\n";
    vs << "precision highp int;\n";
    vs << "precision highp sampler2D;\n";
    vs << "precision highp samplerCube;\n";
    vs << "\n";
    vs << "layout(location = 0) in vec2 position;\n";
    vs << "\n";
    vs << "out vec2 out_uv;\n";
    vs << "\n";
    vs << "void main() {\n";
    vs << "    gl_Position = vec4(position, 0.0, 1.0);\n";
    vs << "    out_uv = position* 0.5 + 0.5;\n";
    vs << "}";
    set_vertex_shader(vs.str());

    // Fragment Shader ---------------------------------------------------------------------------------
    std::stringstream fs;
    fs << "#version 300 es\n";
    fs << "\n";
    fs << "#define GX_PI 3.141592653589793238\n";
    fs << "\n";
    fs << "precision highp float;\n";
    fs << "precision highp int;\n";
    fs << "precision highp sampler2D;\n";
    fs << "precision highp samplerCube;\n";
    fs << "\n";
    fs << "uniform vec2 screen_space_uv;\n";
    fs << "\n";
    fs << "uniform sampler2D source_texture;\n";
    fs << "\n";
    fs << "in vec2 out_uv;\n";
    fs << "\n";
    fs << "out vec3 frag_colour;\n";
    fs << "\n";
    fs << "void main() {\n";
    fs << "    float tex_offset = 0.0;\n";
    fs << "    frag_colour = texture(source_texture, out_uv).xyz * " << gaussian_weights[0] << ";\n";
    for (std::size_t i = 1; i < gaussian_weights_count; ++i) {
        fs << "    tex_offset += " << (is_horizontal ? "screen_space_uv.x" : "screen_space_uv.y") << ";\n";
        fs << "    frag_colour += texture(source_texture, vec2("
           << (is_horizontal ? "out_uv.x + tex_offset, out_uv.y" : "out_uv.x, out_uv.y + tex_offset") << ")).xyz * "
           << gaussian_weights[i] << ";\n";
        fs << "    frag_colour += texture(source_texture, vec2("
           << (is_horizontal ? "out_uv.x - tex_offset, out_uv.y" : "out_uv.x, out_uv.y - tex_offset") << ")).xyz * "
           << gaussian_weights[i] << ";\n";
    }
    fs << "}\n";
    set_fragment_shader(fs.str());

    link();
    GX_GL_SHADER_SET_TEXTURE_INDEX_STARTING;
    GX_GL_THIS_GET_UNIFORM(screen_space_uv);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(source_texture);
}

gearoenix::gl::shader::Bloom::~Bloom() noexcept = default;

void gearoenix::gl::shader::Bloom::bind(uint& current_shader) const noexcept
{
    if (shader_program == current_shader)
        return;
    Shader::bind(current_shader);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(source_texture);
}

gearoenix::gl::shader::BloomCombination::BloomCombination(Engine& e) noexcept
    : e(e)
{
}

#endif