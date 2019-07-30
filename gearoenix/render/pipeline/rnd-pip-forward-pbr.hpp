#ifndef GEAROENIX_RENDER_PIPELINE_FORWARD_PBR_DIRECTIONAL_HPP
#define GEAROENIX_RENDER_PIPELINE_FORWARD_PBR_DIRECTIONAL_HPP
#include "rnd-pip-pipeline.hpp"
namespace gearoenix::render::pipeline {
class ForwardPbr : public Pipeline {
protected:
    ForwardPbr(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
        : Pipeline(Type::ForwardPbr, e, call)
    {
    }

public:
    virtual ~ForwardPbr() noexcept = default;
};
}
#endif
