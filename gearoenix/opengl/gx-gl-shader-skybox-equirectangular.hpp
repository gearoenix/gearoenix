#ifndef GEAROENIX_GL_SHADER_SKYBOX_EQUIRECTANGULAR_HPP
#define GEAROENIX_GL_SHADER_SKYBOX_EQUIRECTANGULAR_HPP
#include "gx-gl-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED

namespace gearoenix::gl {
struct ShaderSkyboxEquirectangular final : public Shader {
    GX_GL_UNIFORM_MATRIX(vp, 4, 1)
    GX_GL_UNIFORM_VECTOR(camera_position_box_scale, 4, 1)
    GX_GL_UNIFORM_TEXTURE(albedo)

    ShaderSkyboxEquirectangular(Engine& e) noexcept;
    ~ShaderSkyboxEquirectangular() noexcept final;
    void bind() const noexcept final;
};
}

#endif
#endif