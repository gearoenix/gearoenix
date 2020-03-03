#ifndef GEAROENIX_RENDER_PIPELINE_RADIANCE_CONVOLUTER_HPP
#define GEAROENIX_RENDER_PIPELINE_RADIANCE_CONVOLUTER_HPP
#include "rnd-pip-pipeline.hpp"
namespace gearoenix::render::pipeline {
class RadianceConvoluter : public Pipeline {
protected:
    RadianceConvoluter(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
        : Pipeline(Type::RadianceConvoluter, e, call)
    {
    }

public:
    ~RadianceConvoluter() noexcept override = default;
};
}
#endif
