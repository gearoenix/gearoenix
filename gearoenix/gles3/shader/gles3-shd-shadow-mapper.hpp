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
        ShadowMapper(const std::shared_ptr<engine::Engine>& e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c);
        ~ShadowMapper() override final;
        void bind() const override final;
    };
}
}
#endif
#endif
