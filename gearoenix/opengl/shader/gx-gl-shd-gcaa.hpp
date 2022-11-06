#ifndef GEAROENIX_GL_SHADER_GAMA_CORRECTION_COLOUR_TUNING_ANTI_ALIASING_HPP
#define GEAROENIX_GL_SHADER_GAMA_CORRECTION_COLOUR_TUNING_ANTI_ALIASING_HPP
#include "gx-gl-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED

namespace gearoenix::gl::shader {
struct GamaCorrectionColourTuningAntiAliasing final : public Shader {
    GX_GL_UNIFORM_VECTOR(screen_space_uv_exposure_gamma, 4, 1);
    GX_GL_UNIFORM_TEXTURE(low_texture);
    GX_GL_UNIFORM_TEXTURE(high_texture);
    GX_GL_UNIFORM_TEXTURE(depth_texture);

public:
    explicit GamaCorrectionColourTuningAntiAliasing(Engine& e) noexcept;
    ~GamaCorrectionColourTuningAntiAliasing() noexcept final;
    void bind(uint& current_shader) const noexcept final;
};
}

#endif
#endif