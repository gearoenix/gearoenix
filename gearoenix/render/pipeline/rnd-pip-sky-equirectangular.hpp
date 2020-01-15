#ifndef GEAROENIX_RENDER_PIPELINE_UNLIT_HPP
#define GEAROENIX_RENDER_PIPELINE_UNLIT_HPP
#include "rnd-pip-pipeline.hpp"
namespace gearoenix::render::pipeline {
class Unlit : public Pipeline {
protected:
    Unlit(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
        : Pipeline(Type::Unlit, e, call)
    {
    }

public:
    ~Unlit() noexcept override = default;
};
}
#endif
