#pragma once
#include "gx-gl-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED

namespace gearoenix::gl::shader {
struct Multiply final : Shader {
    GX_GL_UNIFORM_VECTOR(value_mip_index, 4, 1);
    GX_GL_UNIFORM_TEXTURE(source_texture);

public:
    Multiply();
    ~Multiply() override;
    void bind(uint& current_shader) const override;
};
}
#endif