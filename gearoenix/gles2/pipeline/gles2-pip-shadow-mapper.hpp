#ifndef GEAROENIX_GLES2_PIPELINE_SHADOW_MAPPER_HPP
#define GEAROENIX_GLES2_PIPELINE_SHADOW_MAPPER_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../render/pipeline/rnd-pip-shadow-mapper.hpp"

namespace gearoenix::gles2::engine {
class Engine;
}

namespace gearoenix::gles2::shader {
class ShadowMapper;
}

namespace gearoenix::gles2::pipeline {
class ShadowMapper : public render::pipeline::ShadowMapper, public std::enable_shared_from_this<ShadowMapper> {
private:
    const std::shared_ptr<shader::ShadowMapper> shd;

public:
    ShadowMapper(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    ~ShadowMapper() noexcept final;
    [[nodiscard]] render::pipeline::ResourceSet* create_resource_set() const noexcept final;
};
}

#endif
#endif