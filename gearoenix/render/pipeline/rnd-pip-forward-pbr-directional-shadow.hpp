#ifndef GEAROENIX_RENDER_PIPELINE_FORWARD_PBR_DIRECTIONAL_HPP
#define GEAROENIX_RENDER_PIPELINE_FORWARD_PBR_DIRECTIONAL_HPP
#include "rnd-pip-pipeline.hpp"
namespace gearoenix::render::pipeline {
class ForwardPbrDirectionalShadow : public Pipeline {
protected:
    ForwardPbrDirectionalShadow(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
        : Pipeline(Type::ForwardPbrDirectionalShadow, e, call)
    {
    }

public:
    virtual ~ForwardPbrDirectionalShadow() noexcept = default;
};
}
#endif
