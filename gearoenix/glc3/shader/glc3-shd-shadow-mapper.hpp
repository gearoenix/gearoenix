#ifndef GEAROENIX_GLC3_SHADER_SHADOW_MAPPER_HPP
#define GEAROENIX_GLC3_SHADER_SHADOW_MAPPER_HPP
#include "glc3-shd-shader.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
namespace gearoenix::glc3 {
namespace engine {
    class Engine;
}
namespace shader {
    class ShadowMapper : public Shader {
        GX_GLC3_SHADER_MATERIAL_UNIFORMS
        GX_GLC3_UNIFORM_MATRIX(effect_mvp, 4, 1)
    public:
        ShadowMapper(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
        ~ShadowMapper() noexcept final;
        void bind() const noexcept final;
    };
}
}
#endif
#endif
