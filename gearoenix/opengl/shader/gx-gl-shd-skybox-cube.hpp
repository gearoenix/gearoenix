#ifndef GEAROENIX_GL_SHADER_SKYBOX_CUBE_HPP
#define GEAROENIX_GL_SHADER_SKYBOX_CUBE_HPP
#include "gx-gl-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED

namespace gearoenix::gl::shader {
struct SkyboxCube final : public Shader {
    GX_GL_UNIFORM_MATRIX(vp, 4, 1);
    GX_GL_UNIFORM_VECTOR(camera_position_box_scale, 4, 1);
    GX_GL_UNIFORM_TEXTURE(albedo);

public:
    explicit SkyboxCube(Engine& e);
    ~SkyboxCube() final;
    void bind(uint& current_shader) const final;
};
}

#endif
#endif