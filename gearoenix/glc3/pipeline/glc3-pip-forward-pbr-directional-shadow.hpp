#ifndef GEAROENIX_GLC3_PIPELINE_FORWARD_PBR_DIRECTIONAL_SHADOW_HPP
#define GEAROENIX_GLC3_PIPELINE_FORWARD_PBR_DIRECTIONAL_SHADOW_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../render/pipeline/rnd-pip-forward-pbr-directional-shadow.hpp"
namespace gearoenix::glc3 {
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
        ForwardPbrDirectionalShadow(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
        render::pipeline::ResourceSet* create_resource_set() const noexcept final;
    };
}
}
#endif
#endif
