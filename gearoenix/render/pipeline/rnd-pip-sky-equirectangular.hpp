#ifndef GEAROENIX_RENDER_PIPELINE_SKY_EQUIRECTANGULAR_HPP
#define GEAROENIX_RENDER_PIPELINE_SKY_EQUIRECTANGULAR_HPP
#include "rnd-pip-pipeline.hpp"
namespace gearoenix::render::pipeline {
class SkyEquirectangular : public Pipeline {
protected:
    SkyEquirectangular(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
        : Pipeline(Type::SkyEquirectangular, e, call)
    {
    }

public:
    ~SkyEquirectangular() noexcept override = default;
};
}
#endif
