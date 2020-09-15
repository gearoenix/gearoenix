#ifndef GEAROENIX_GLC3_PIPELINE_FORWARD_PBR_HPP
#define GEAROENIX_GLC3_PIPELINE_FORWARD_PBR_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../render/pipeline/gx-rnd-pip-forward-pbr.hpp"

namespace gearoenix::glc3::engine {
class Engine;
}

namespace gearoenix::glc3::shader {
class ForwardPbr;
}

namespace gearoenix::glc3::pipeline {
class ForwardPbr final : public render::pipeline::ForwardPbr {
private:
    std::weak_ptr<ForwardPbr> self;
    const std::shared_ptr<shader::ForwardPbr> shd;

    ForwardPbr(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;

public:
    [[nodiscard]] static std::shared_ptr<ForwardPbr> construct(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    ~ForwardPbr() noexcept final;
    [[nodiscard]] render::pipeline::ResourceSet* create_resource_set() const noexcept final;
};
}

#endif
#endif
