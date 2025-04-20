#pragma once
#include "gx-gl-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED

namespace gearoenix::gl::shader {
struct Irradiance final : Shader {
    GX_GL_UNIFORM_MATRIX(m, 3, 1);
    GX_GL_UNIFORM_TEXTURE(environment);

public:
    Irradiance();
    ~Irradiance() override;
    void bind(uint& current_shader) const override;
};
}

#endif