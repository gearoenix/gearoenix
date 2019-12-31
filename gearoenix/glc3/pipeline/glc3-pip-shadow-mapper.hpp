#ifndef GEAROENIX_GLC3_PIPELINE_SHADOW_MAPPER_HPP
#define GEAROENIX_GLC3_PIPELINE_SHADOW_MAPPER_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../render/pipeline/rnd-pip-shadow-mapper.hpp"

namespace gearoenix::glc3::engine {
class Engine;
}

namespace gearoenix::glc3::shader {
class ShadowMapper;
}

namespace gearoenix::glc3::pipeline {
class ShadowMapper : public render::pipeline::ShadowMapper {
private:
    const std::shared_ptr<shader::ShadowMapper> shd;

public:
    ShadowMapper(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    [[nodiscard]] render::pipeline::ResourceSet* create_resource_set() const noexcept final;
};
}

#endif
#endif