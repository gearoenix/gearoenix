#ifndef GEAROENIX_GL_SHADER_SHADOW_CASTER_HPP
#define GEAROENIX_GL_SHADER_SHADOW_CASTER_HPP
#include "gx-gl-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED

namespace gearoenix::gl::shader {
struct ShadowCaster final : public Shader {
    GX_GL_UNIFORM_MATRIX(mvp, 4, 1)
    GX_GL_UNIFORM_VECTOR(alpha_factor_alpha_cutoff, 2, 1)
    GX_GL_UNIFORM_TEXTURE(albedo)

    ShadowCaster(Engine& e) noexcept;
    ~ShadowCaster() noexcept final;
    void bind() const noexcept final;
};
}

#endif
#endif