#ifndef GEAROENIX_GLES2_SHADER_SHADOW_MAPPER_HPP
#define GEAROENIX_GLES2_SHADER_SHADOW_MAPPER_HPP
#include "gles2-shd-shader.hpp"
#ifdef GX_USE_OPENGL_ES2
namespace gearoenix::gles2 {
namespace engine {
    class Engine;
}
namespace shader {
    class ShadowMapper : public Shader {
        GX_GLES2_SHADER_MATERIAL_UNIFORMS
        GX_GLES2_UNIFORM_MATRIX(effect_mvp, 4, 1)
    public:
        ShadowMapper(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
        ~ShadowMapper() noexcept final;
        void bind() const noexcept final;
    };
}
}
#endif
#endif
