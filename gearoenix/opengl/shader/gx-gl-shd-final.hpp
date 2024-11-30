#pragma once
#include "gx-gl-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED

namespace gearoenix::gl::shader {
struct Final final : public Shader {
    GX_GL_UNIFORM_TEXTURE(albedo);

public:
    explicit Final(Engine& e);
    ~Final() final;
    void bind(uint& current_shader) const final;
};
}

#endif