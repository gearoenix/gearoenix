#pragma once
#include "gx-gl-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED

namespace gearoenix::gl::shader {
struct GBuffersFiller final : Shader {
    GX_GL_UNIFORM_MATRIX(m, 4, 1);
    GX_GL_UNIFORM_MATRIX(inv_m, 4, 1);
    GX_GL_UNIFORM_MATRIX(vp, 4, 1);
    GX_GL_UNIFORM_VECTOR(albedo_factor, 4, 1);
    GX_GL_UNIFORM_VECTOR(normal_metallic_factor, 4, 1);
    GX_GL_UNIFORM_VECTOR(emission_roughness_factor, 4, 1);
    GX_GL_UNIFORM_VECTOR(alpha_cutoff_occlusion_strength_radiance_lod_coefficient_reserved, 4, 1);
    GX_GL_UNIFORM_VECTOR(camera_position, 3, 1);
    GX_GL_UNIFORM_TEXTURE(albedo);
    GX_GL_UNIFORM_TEXTURE(normal);
    GX_GL_UNIFORM_TEXTURE(emission);
    GX_GL_UNIFORM_TEXTURE(metallic_roughness);
    GX_GL_UNIFORM_TEXTURE(occlusion);
    GX_GL_UNIFORM_TEXTURE(irradiance);
    GX_GL_UNIFORM_TEXTURE(radiance);

public:
    explicit GBuffersFiller(Engine& e);
    ~GBuffersFiller() override;
    void bind(uint& current_shader) const override;
};
}

#endif