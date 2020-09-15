#ifndef GEAROENIX_RENDER_PIPELINE_IRRADIANCE_CONVOLUTER_HPP
#define GEAROENIX_RENDER_PIPELINE_IRRADIANCE_CONVOLUTER_HPP
#include "gx-rnd-pip-pipeline.hpp"
namespace gearoenix::render::pipeline {
class IrradianceConvoluter : public Pipeline {
protected:
    IrradianceConvoluter(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
        : Pipeline(Type::IrradianceConvoluter, e, call)
    {
    }

public:
    ~IrradianceConvoluter() noexcept override = default;
};
}
#endif
