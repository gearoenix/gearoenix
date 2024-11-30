#pragma once
#include "gx-gl-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED

namespace gearoenix::gl::shader {
struct SsaoResolve final : Shader {
    GX_GL_UNIFORM_MATRIX(vp, 4, 1);
    GX_GL_UNIFORM_VECTOR(ssao_radius_move_start_end, 4, 1);
    GX_GL_UNIFORM_TEXTURE(position_depth);
    GX_GL_UNIFORM_TEXTURE(normal_ao);

public:
    explicit SsaoResolve(Engine& e);
    ~SsaoResolve() override;
    void bind(uint& current_shader) const override;
};
}

#endif