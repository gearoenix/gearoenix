#ifndef GEAROENIX_RENDER_PIPELINE_SKY_CUBE_HPP
#define GEAROENIX_RENDER_PIPELINE_SKY_CUBE_HPP
#include "gx-rnd-pip-pipeline.hpp"
namespace gearoenix::render::pipeline {
class SkyboxCube : public Pipeline {
protected:
    SkyboxCube(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
        : Pipeline(Type::SkyboxCube, e, call)
    {
    }

public:
    ~SkyboxCube() noexcept override = default;
};
}
#endif
