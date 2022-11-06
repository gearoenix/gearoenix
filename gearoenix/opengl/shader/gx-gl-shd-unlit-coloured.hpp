#ifndef GEAROENIX_GL_SHADER_UNLIT_COLOURED_HPP
#define GEAROENIX_GL_SHADER_UNLIT_COLOURED_HPP
#include "gx-gl-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED

namespace gearoenix::gl::shader {
struct UnlitColoured final : public Shader {
    GX_GL_UNIFORM_MATRIX(mvp, 4, 1);
    GX_GL_UNIFORM_VECTOR(colour, 3, 1);

public:
    explicit UnlitColoured(Engine& e) noexcept;
    ~UnlitColoured() noexcept final;
    void bind(uint& current_shader) const noexcept final;
};
}

#endif
#endif