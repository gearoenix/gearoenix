#include "gx-gl-shd-shadow-caster.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../gx-gl-engine.hpp"
#include <sstream>

gearoenix::gl::shader::ShadowCaster::ShadowCaster(Engine& e, const std::size_t bones_count) noexcept
    : Shader(e)
    , mvp_count(static_cast<sizei>(bones_count + 1))
{
    std::stringstream vs;
    vs << "#version 300 es\n";
    vs << "\n";
    vs << "precision highp float;\n";
    vs << "\n";
    vs << "layout(location = " << GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_POSITION << ") in vec3 position;\n";
    vs << "layout(location = " << GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_UV << ") in vec2 uv;\n";
    if (mvp_count < 2) {
        vs << "\n";
        vs << "uniform mat4 mvp;\n";
    } else {
        vs << "layout(location = " << GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_BONE_WEIGHTS << ") in vec4 bones_weight;\n";
        vs << "layout(location = " << GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_BONE_INDICES << ") in vec4 bones_indices;\n";
        vs << "\n";
        vs << "uniform mat4 mvp[" << mvp_count << "];\n";
    }
    vs << "\n";
    vs << "out vec2 out_uv;\n";
    vs << "\n";
    vs << "void main() {\n";
    vs << "    out_uv = uv;\n";
    if (mvp_count < 2) {
        vs << "    gl_Position = mvp * vec4(position, 1.0);\n";
    } else {
        vs << "    ivec4 bi = ivec4(bones_indices);\n";
        vs << "    gl_Position = (\n";
        vs << "        (mvp[bi.x] * bones_weight.x) +\n";
        vs << "        (mvp[bi.y] * bones_weight.y) +\n";
        vs << "        (mvp[bi.z] * bones_weight.z) +\n";
        vs << "        (mvp[bi.w] * bones_weight.w)) * vec4(position, 1.0);\n";
    }
    vs << "}\n";
    set_vertex_shader(vs.str());
    std::stringstream fs;
    fs << "#version 300 es\n";
    fs << "\n";
    fs << "precision highp float;\n";
    fs << "\n";
    fs << "uniform vec2 alpha_factor_alpha_cutoff;\n";
    fs << "\n";
    fs << "uniform sampler2D albedo;\n";
    fs << "\n";
    fs << "in vec2 out_uv;\n";
    fs << "\n";
    fs << "out vec3 frag_colour;\n";
    fs << "\n";
    fs << "void main() {\n";
    fs << "\n";
    fs << "    float alpha = texture(albedo, out_uv).w * alpha_factor_alpha_cutoff.x;\n";
    fs << "    if(alpha <= alpha_factor_alpha_cutoff.y) discard;\n";
    fs << "    frag_colour = vec3(1.0);\n";
    fs << "}\n";
    set_fragment_shader(fs.str());
    link();
    GX_GL_SHADER_SET_TEXTURE_INDEX_STARTING;
    GX_GL_THIS_GET_UNIFORM(mvp);
    GX_GL_THIS_GET_UNIFORM(alpha_factor_alpha_cutoff);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(albedo);
}

gearoenix::gl::shader::ShadowCaster::~ShadowCaster() noexcept = default;

void gearoenix::gl::shader::ShadowCaster::bind() const noexcept
{
    Shader::bind();
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(albedo);
}

void gearoenix::gl::shader::ShadowCaster::set_mvp_data(const void* const data) const noexcept
{
    glUniformMatrix4fv(mvp, mvp_count, GL_FALSE, reinterpret_cast<const float*>(data));
}

gearoenix::gl::shader::ShadowCasterBoneCountCombination::ShadowCasterBoneCountCombination(Engine& e) noexcept
{
    for (std::size_t i = 0; i < decltype(shaders)::max_size(); ++i)
        shaders.emplace_back(e, i);
}

const gearoenix::gl::shader::ShadowCaster& gearoenix::gl::shader::ShadowCasterBoneCountCombination::get_shader(const std::size_t bones_count) const noexcept
{
    return shaders[bones_count];
}

#endif
