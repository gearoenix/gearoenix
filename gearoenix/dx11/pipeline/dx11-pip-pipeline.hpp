#ifndef GEAROENIX_DX11_PIPELINE_PIPELINE_HPP
#define GEAROENIX_DX11_PIPELINE_PIPELINE_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_DIRECTX11
#include "../../render/pipeline/rnd-pip-pipeline.hpp"
#include <memory>
namespace gearoenix {
namespace dx11 {
    class Engine;
    namespace shader {
        class Shader;
    }
    namespace pipeline {
        class Pipeline : public render::pipeline::Pipeline {
        private:
        public:
            Pipeline(
                core::Id sid,
                Engine* eng,
                core::sync::EndCaller<core::sync::EndCallerIgnore> call);
            ~Pipeline();
            void bind();
        };
    }
}
}
#endif
#endif
