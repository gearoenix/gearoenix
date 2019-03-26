#ifndef GEAROENIX_GLES2_SHADER_FORWARD_PBR_DIRECTIONAL_SHADOW_HPP
#define GEAROENIX_GLES2_SHADER_FORWARD_PBR_DIRECTIONAL_SHADOW_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "gles2-shd-shader.hpp"
#include "../gles2.hpp"
#include <string>
namespace gearoenix {
namespace gles2 {
	namespace engine {
		class Engine;
	}
    namespace shader {
        class ForwardPbrDirectionalShadow : public Shader {
        public:
			ForwardPbrDirectionalShadow(const std::shared_ptr<engine::Engine> &e, const core::sync::EndCaller<core::sync::EndCallerIgnore> &c);
            ~ForwardPbrDirectionalShadow() override final;
        };
    }
}
}
#endif
#endif
