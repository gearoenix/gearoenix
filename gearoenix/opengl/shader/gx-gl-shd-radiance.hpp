#ifndef GEAROENIX_GL_SHADER_RADIANCE_HPP
#define GEAROENIX_GL_SHADER_RADIANCE_HPP
#include "gx-gl-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED

namespace gearoenix::gl::shader {
struct Radiance final : public Shader {
    GX_GL_UNIFORM_FLOAT(roughness, 1)
    GX_GL_UNIFORM_FLOAT(roughness_p_4, 1)
    GX_GL_UNIFORM_FLOAT(sa_texel, 1)
    GX_GL_UNIFORM_MATRIX(m, 3, 1)
    GX_GL_UNIFORM_TEXTURE(environment)

    Radiance(Engine& e) noexcept;
    ~Radiance() noexcept final;
    void bind() const noexcept final;
};
}

#endif
#endif