#ifndef GEAROENIX_GL_SHADER_FINAL_HPP
#define GEAROENIX_GL_SHADER_FINAL_HPP
#include "gx-gl-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED

namespace gearoenix::gl::shader {
struct Final final : public Shader {
    GX_GL_UNIFORM_TEXTURE(albedo);

public:
    explicit Final(Engine& e) noexcept;
    ~Final() noexcept final;
    void bind() const noexcept final;
};
}

#endif
#endif