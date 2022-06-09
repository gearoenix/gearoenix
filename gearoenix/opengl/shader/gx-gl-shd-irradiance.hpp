#ifndef GEAROENIX_GL_SHADER_IRRADIANCE_HPP
#define GEAROENIX_GL_SHADER_IRRADIANCE_HPP
#include "gx-gl-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED

namespace gearoenix::gl::shader {
struct Irradiance final : public Shader {
    GX_GL_UNIFORM_MATRIX(m, 3, 1)
    GX_GL_UNIFORM_TEXTURE(environment)

    Irradiance(Engine& e) noexcept;
    ~Irradiance() noexcept final;
    void bind() const noexcept final;
};
}

#endif
#endif