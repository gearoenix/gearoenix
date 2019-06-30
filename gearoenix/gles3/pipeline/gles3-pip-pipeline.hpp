#ifndef GEAROENIX_GLES3_PIPELINE_PIPELINE_HPP
#define GEAROENIX_GLES3_PIPELINE_PIPELINE_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES3
#include "../../render/pipeline/rnd-pip-pipeline.hpp"
#include <memory>
namespace gearoenix::gles3 {
namespace engine {
    class Engine;
}
namespace shader {
    class Shader;
}
namespace pipeline {
    class Pipeline : public render::pipeline::Pipeline {
    private:
    public:
        Pipeline(
            const render::pipeline::Type::Id pid,
            const std::shared_ptr<engine::Engine>& e,
            const core::sync::EndCaller<core::sync::EndCallerIgnore>& call);
        ~Pipeline();
        void bind();
    };
}
}
#endif
#endif
