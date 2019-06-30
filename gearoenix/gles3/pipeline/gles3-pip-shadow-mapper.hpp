#ifndef GEAROENIX_GLES3_PIPELINE_SHADOW_MAPPER_HPP
#define GEAROENIX_GLES3_PIPELINE_SHADOW_MAPPER_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES3
#include "../../render/pipeline/rnd-pip-shadow-mapper.hpp"
namespace gearoenix::gles3 {
namespace engine {
    class Engine;
}
namespace shader {
    class ShadowMapper;
}
namespace pipeline {
    class ShadowMapper : public render::pipeline::ShadowMapper {
    private:
        const std::shared_ptr<shader::ShadowMapper> shd;

    public:
        ShadowMapper(const std::shared_ptr<engine::Engine>& e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c);
        render::pipeline::ResourceSet* create_resource_set() const override final;
    };
}
}
#endif
#endif