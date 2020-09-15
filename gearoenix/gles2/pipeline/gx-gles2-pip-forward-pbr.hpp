#ifndef GEAROENIX_GLES2_PIPELINE_FORWARD_PBR_HPP
#define GEAROENIX_GLES2_PIPELINE_FORWARD_PBR_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../render/pipeline/gx-rnd-pip-forward-pbr.hpp"

namespace gearoenix::gles2::engine {
class Engine;
}

namespace gearoenix::gles2::shader {
class ForwardPbr;
}

namespace gearoenix::gles2::pipeline {
class ForwardPbr final : public render::pipeline::ForwardPbr {
private:
    const std::shared_ptr<shader::ForwardPbr> shd;
    std::weak_ptr<ForwardPbr> self;
    ForwardPbr(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;

public:
    [[nodiscard]] static std::shared_ptr<ForwardPbr> construct(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    ~ForwardPbr() noexcept final;
    [[nodiscard]] render::pipeline::ResourceSet* create_resource_set() const noexcept final;
};
}

#endif
#endif
