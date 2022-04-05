#ifndef GEAROENIX_GL_GBUFFERS_FILLER_SHADER_HPP
#define GEAROENIX_GL_GBUFFERS_FILLER_SHADER_HPP
#include "gx-gl-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED

namespace gearoenix::gl {
struct GBuffersFillerShader final : public Shader {
    GX_GL_UNIFORM_MATRIX(m, 4, 1)
    GX_GL_UNIFORM_MATRIX(inv_m, 4, 1)
    GX_GL_UNIFORM_MATRIX(vp, 4, 1)
    GX_GL_UNIFORM_VECTOR(camera_position, 3, 1)
    GX_GL_UNIFORM_VECTOR(albedo_factor, 4, 1)
    GX_GL_UNIFORM_VECTOR(normal_metallic_factor, 4, 1)
    GX_GL_UNIFORM_VECTOR(emission_roughness_factor, 4, 1)
    GX_GL_UNIFORM_VECTOR(alpha_cutoff_occlusion_strength_radiance_lod_coefficient_reserved, 4, 1)
    GX_GL_UNIFORM_TEXTURE(albedo)
    GX_GL_UNIFORM_TEXTURE(normal)
    GX_GL_UNIFORM_TEXTURE(emission)
    GX_GL_UNIFORM_TEXTURE(metallic_roughness)
    GX_GL_UNIFORM_TEXTURE(occlusion)

    GBuffersFillerShader(Engine& e) noexcept;
    ~GBuffersFillerShader() noexcept final;
    void bind() const noexcept final;
};
}

#endif
#endif