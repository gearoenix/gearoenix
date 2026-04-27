#pragma once
#include "gx-gl-shader.hpp"
#if GX_RENDER_OPENGL_ENABLED

namespace gearoenix::gl::shader {
/// Trivial fullscreen-triangle copy from a 2D texture to the bound framebuffer. Used by the
/// submission manager to bring each camera's already-tonemapped, sRGB-encoded output (in the
/// camera's RGBA16F target) onto the default framebuffer with the right letterbox rect.
/// Doesn't apply any colour transform -- the colour-tuning pass already produced display-referred
/// signal.
struct Final final : Shader {
    GX_GL_UNIFORM_TEXTURE(source);

public:
    Final();
    ~Final() override;
    void bind(uint& current_shader) const override;
};
}

#endif
