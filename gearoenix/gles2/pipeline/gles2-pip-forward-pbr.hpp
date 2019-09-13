#ifndef GEAROENIX_GLES2_PIPELINE_FORWARD_PBR_HPP
#define GEAROENIX_GLES2_PIPELINE_FORWARD_PBR_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../render/pipeline/rnd-pip-forward-pbr.hpp"
namespace gearoenix::gles2 {
namespace engine {
    class Engine;
}
namespace shader {
    class ForwardPbr;
}
namespace pipeline {
    class ForwardPbr : public render::pipeline::ForwardPbr {
    private:
        const std::shared_ptr<shader::ForwardPbr> shd;

    public:
        ForwardPbr(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
        render::pipeline::ResourceSet* create_resource_set() const noexcept final;
    };
}
}
#endif
#endif
