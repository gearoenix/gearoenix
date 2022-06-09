#ifndef GEAROENIX_GL_SHADER_SSAO_RESOLVE_HPP
#define GEAROENIX_GL_SHADER_SSAO_RESOLVE_HPP
#include "gx-gl-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED

namespace gearoenix::gl::shader {
struct SsaoResolve final : public Shader {
    GX_GL_UNIFORM_MATRIX(vp, 4, 1)
    GX_GL_UNIFORM_VECTOR(ssao_radius_move_start_end, 4, 1)
    GX_GL_UNIFORM_TEXTURE(position_depth)
    GX_GL_UNIFORM_TEXTURE(normal_ao)

    SsaoResolve(Engine& e) noexcept;
    ~SsaoResolve() noexcept final;
    void bind() const noexcept final;
};
}

#endif
#endif