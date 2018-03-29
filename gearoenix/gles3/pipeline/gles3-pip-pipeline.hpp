#ifndef GEAROENIX_gles3_PIPELINE_PIPELINE_HPP
#define GEAROENIX_gles3_PIPELINE_PIPELINE_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL_ES3
#include "../../render/pipeline/rnd-pip-pipeline.hpp"
#include <memory>
namespace gearoenix {
namespace gles3 {
    class Engine;
    namespace shader {
        class Shader;
    }
    namespace pipeline {
        class Pipeline : public render::pipeline::Pipeline {
        private:
        public:
            Pipeline(core::Id sid, Engine* eng, core::sync::EndCaller<core::sync::EndCallerIgnore> call);
            ~Pipeline();
            void bind();
        };
    }
}
}
#endif
#endif
