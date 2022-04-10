#ifndef GEAROENIX_GL_FINAL_EFFECTS_SHADER_HPP
#define GEAROENIX_GL_FINAL_EFFECTS_SHADER_HPP
#include "gx-gl-shader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED

namespace gearoenix::gl {
struct FinalEffectsShader final : public Shader {
    GX_GL_UNIFORM_TEXTURE(position_depth)
    GX_GL_UNIFORM_TEXTURE(albedo_metallic)
    GX_GL_UNIFORM_TEXTURE(normal_roughness)
    GX_GL_UNIFORM_TEXTURE(emission_ambient_occlusion)

    FinalEffectsShader(Engine& e) noexcept;
    ~FinalEffectsShader() noexcept final;
    void bind() const noexcept final;
};
}

#endif
#endif