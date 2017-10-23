#ifndef GEAROENIX_GLES2_PIPELINE_PIPELINE_HPP
#define GEAROENIX_GLES2_PIPELINE_PIPELINE_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL_ES2
#include "../../render/pipeline/rnd-pip-pipeline.hpp"
namespace gearoenix {
namespace gles2 {
    class Engine;
    namespace pipeline {
        class Pipeline : public render::pipeline::Pipeline {
        private:
        public:
            Pipeline(core::Id sid, Engine* eng);
            virtual ~Pipeline();
            virtual void bind() = 0;
        };
    }
}
}
#endif
#endif
