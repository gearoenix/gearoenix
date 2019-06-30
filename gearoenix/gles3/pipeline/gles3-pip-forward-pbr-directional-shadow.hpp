#ifndef GEAROENIX_GLES3_PIPELINE_FORWARD_PBR_DIRECTIONAL_SHADOW_HPP
#define GEAROENIX_GLES3_PIPELINE_FORWARD_PBR_DIRECTIONAL_SHADOW_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES3
#include "../../render/pipeline/rnd-pip-forward-pbr-directional-shadow.hpp"
namespace gearoenix::gles3 {
    namespace engine {
        class Engine;
    }
    namespace shader {
        class ForwardPbrDirectionalShadow;
    }
    namespace pipeline {
        class ForwardPbrDirectionalShadow : public render::pipeline::ForwardPbrDirectionalShadow {
        private:
            const std::shared_ptr<shader::ForwardPbrDirectionalShadow> shd;

        public:
            ForwardPbrDirectionalShadow(const std::shared_ptr<engine::Engine>& e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c);
            render::pipeline::ResourceSet* create_resource_set() const override final;
        };
    }
}
#endif
#endif
