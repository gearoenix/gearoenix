#include "gx-gl-shd-unlit.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED

gearoenix::gl::shader::Unlit::Unlit(const bool has_uv_transform, const bool has_alpha_cutoff, const bool has_albedo_factor, const bool has_albedo)
{
    std::stringstream vs;
    vs << get_common_shader_starter();
    vs << "layout(location = " << GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_POSITION << ") in vec3 position;\n";
    if (has_albedo) {
        vs << "layout(location = " << GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_UV << ") in vec2 uv;\n";
        vs << "out vec2 out_uv;\n";
        if (has_uv_transform) {
            vs << "uniform vec4 uv_transform;\n";
        }
    }
    vs << "uniform mat4 mvp;\n";
    vs << "void main() {\n";
    if (has_albedo) {
        if (has_uv_transform) {
            vs << "    out_uv = (uv_transform.xy * uv) + uv_transform.zw;\n";
        } else {
            vs << "    out_uv = uv;\n";
        }
    }
    vs << "    gl_Position = mvp * vec4(position, 1.0);\n";
    vs << "}\n";
    set_vertex_shader(vs.str());

    std::stringstream fs;
    fs << get_common_shader_starter();
    if (has_albedo) {
        fs << "uniform sampler2D albedo;\n";
        fs << "in vec2 out_uv;\n";
    }
    if (has_albedo_factor) {
        fs << "uniform vec4 albedo_factor;\n";
    }
    if (has_alpha_cutoff) {
        fs << "uniform float alpha_cutoff;\n";
    }
    fs << "out vec4 frag_colour;\n";
    fs << "void main() {\n";
    fs << "    vec4 colour = vec4(1.0);\n";
    if (has_albedo) {
        fs << "    colour *= texture(albedo, out_uv);\n";
    }
    if (has_albedo_factor) {
        fs << "    colour *= albedo_factor;\n";
    }
    if (has_alpha_cutoff) {
        fs << "    if(colour.w < alpha_cutoff) discard;\n";
    }
    fs << "    frag_colour = colour;\n";
    fs << "}\n";
    set_fragment_shader(fs.str());
    link();
    GX_GL_THIS_GET_UNIFORM(mvp);
    if (has_uv_transform) {
        GX_GL_THIS_GET_UNIFORM(uv_transform);
    }
    if (has_alpha_cutoff) {
        GX_GL_THIS_GET_UNIFORM(alpha_cutoff);
    }
    if (has_albedo_factor) {
        GX_GL_THIS_GET_UNIFORM(albedo_factor);
    }
    if (has_albedo) {
        GX_GL_SHADER_SET_TEXTURE_INDEX_STARTING;
        GX_GL_THIS_GET_UNIFORM_TEXTURE(albedo);
    }
}

gearoenix::gl::shader::Unlit::~Unlit() = default;

void gearoenix::gl::shader::Unlit::bind(uint& current_shader) const
{
    if (shader_program == current_shader)
        return;
    Shader::bind(current_shader);
    if (GX_GL_TEXTURE_INDEX_FAILED != albedo) {
        GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(albedo);
    }
}

#endif