#ifndef GEAROENIX_GL_SHADER_EXPOSURE_HPP
#define GEAROENIX_GL_SHADER_EXPOSURE_HPP
#include "gx-gl-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED

namespace gearoenix::gl::shader {
struct Exposure final : public Shader {
    GX_GL_UNIFORM_FLOAT(value, 1);
    GX_GL_UNIFORM_TEXTURE(source_texture);

public:
    explicit Exposure(Engine& e);
    ~Exposure() final;
    void bind(uint& current_shader) const final;
};
}
#endif
#endif