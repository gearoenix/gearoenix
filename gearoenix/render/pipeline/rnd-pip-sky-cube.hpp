#ifndef GEAROENIX_RENDER_PIPELINE_SKY_CUBE_HPP
#define GEAROENIX_RENDER_PIPELINE_SKY_CUBE_HPP
#include "rnd-pip-pipeline.hpp"
namespace gearoenix::render::pipeline {
class SkyCube : public Pipeline {
protected:
    SkyCube(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
        : Pipeline(Type::SkyCube, e, call)
    {
    }

public:
    ~SkyCube() noexcept override = default;
};
}
#endif
