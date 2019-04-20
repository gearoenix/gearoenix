#ifndef GEAROENIX_GLES2_SHADER_FORWARD_PBR_DIRECTIONAL_SHADOW_HPP
#define GEAROENIX_GLES2_SHADER_FORWARD_PBR_DIRECTIONAL_SHADOW_HPP
#include "gles2-shd-shader.hpp"
#ifdef GX_USE_OPENGL_ES2
namespace gearoenix {
namespace gles2 {
    namespace engine {
        class Engine;
    }
    namespace shader {
        class ShadowMapper : public Shader {
        private:
            GX_GLES2_SHADER_MATERIAL_UNIFORMS_LOCATIONS
            gl::sint effect_mvp = GX_SHADER_UNIFORM_FAILED;

        public:
            ShadowMapper(const std::shared_ptr<engine::Engine>& e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c);
            ~ShadowMapper() override final;
            void bind() const override final;
            GX_GLES2_SHADER_MATERIAL_FUNCTION_DECLARATIONS
            GX_GLES2_SHADER_SET_DATA_FUNCTION_M(effect_mvp, 4, 1)
        };
    }
}
}
#endif
#endif
