#ifndef GEAROENIX_GLC3_PIPELINE_IRRADIANCE_CONVOLUTER_HPP
#define GEAROENIX_GLC3_PIPELINE_IRRADIANCE_CONVOLUTER_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../render/pipeline/rnd-pip-irradiance-convoluter.hpp"

namespace gearoenix::glc3::engine {
class Engine;
}

namespace gearoenix::glc3::shader {
class IrradianceConvoluter;
}

namespace gearoenix::glc3::pipeline {
class IrradianceConvoluter final : public render::pipeline::IrradianceConvoluter {
private:
    const std::shared_ptr<shader::IrradianceConvoluter> shd;
    std::weak_ptr<IrradianceConvoluter> self;
    IrradianceConvoluter(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;

public:
    [[nodiscard]] static std::shared_ptr<IrradianceConvoluter> construct(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    ~IrradianceConvoluter() noexcept final;
    [[nodiscard]] render::pipeline::ResourceSet* create_resource_set() const noexcept final;
};
}

#endif
#endif
