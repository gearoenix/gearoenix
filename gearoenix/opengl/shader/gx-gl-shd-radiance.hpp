#pragma once
#include "gx-gl-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED

namespace gearoenix::gl::shader {
struct Radiance final : Shader {
    GX_GL_UNIFORM_FLOAT(roughness, 1);
    GX_GL_UNIFORM_FLOAT(roughness_p_4, 1);
    GX_GL_UNIFORM_FLOAT(sa_texel, 1);
    GX_GL_UNIFORM_MATRIX(m, 3, 1);
    GX_GL_UNIFORM_TEXTURE(environment);

public:
    Radiance();
    ~Radiance() override;
    void bind(uint& current_shader) const override;
};
}

#endif