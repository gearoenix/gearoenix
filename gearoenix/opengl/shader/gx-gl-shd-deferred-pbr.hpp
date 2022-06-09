#ifndef GEAROENIX_GL_SHADER_DEFERRED_PBR_HPP
#define GEAROENIX_GL_SHADER_DEFERRED_PBR_HPP
#include "gx-gl-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED

namespace gearoenix::gl::shader {
struct DeferredPbr final : public Shader {
    GX_GL_UNIFORM_VECTOR(screen_uv_move_reserved, 4, 1)
    GX_GL_UNIFORM_VECTOR(camera_position, 3, 1)
    GX_GL_UNIFORM_TEXTURE(albedo_metallic)
    GX_GL_UNIFORM_TEXTURE(position_depth)
    GX_GL_UNIFORM_TEXTURE(normal_ao)
    GX_GL_UNIFORM_TEXTURE(emission_roughness)
    GX_GL_UNIFORM_TEXTURE(ssao_resolved)
    GX_GL_UNIFORM_TEXTURE(irradiance)
    GX_GL_UNIFORM_TEXTURE(radiance)
    GX_GL_UNIFORM_TEXTURE(brdflut)

    DeferredPbr(Engine& e) noexcept;
    ~DeferredPbr() noexcept final;
    void bind() const noexcept final;
};
}

#endif
#endif