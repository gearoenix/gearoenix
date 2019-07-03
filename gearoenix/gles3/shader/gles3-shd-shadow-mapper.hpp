#ifndef GEAROENIX_GLES3_SHADER_SHADOW_MAPPER_HPP
#define GEAROENIX_GLES3_SHADER_SHADOW_MAPPER_HPP
#include "gles3-shd-shader.hpp"
#ifdef GX_USE_OPENGL_ES3
namespace gearoenix::gles3 {
namespace engine {
    class Engine;
}
namespace shader {
    class ShadowMapper : public Shader {
        GX_GLES3_SHADER_MATERIAL_UNIFORMS
        GX_GLES3_UNIFORM_MATRIX(effect_mvp, 4, 1)
    public:
        ShadowMapper(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
        ~ShadowMapper() noexcept final;
        void bind() const noexcept final;
    };
}
}
#endif
#endif