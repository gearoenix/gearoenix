#ifndef GEAROENIX_GLES2_PIPELINE_MANAGER_HPP
#define GEAROENIX_GLES2_PIPELINE_MANAGER_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../render/pipeline/rnd-pip-manager.hpp"
#include <memory>
namespace gearoenix {
namespace gles2 {
    class Engine;
    namespace shader {
        class Shader;
    }
    namespace pipeline {
        class Manager : public render::pipeline::Manager {
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
