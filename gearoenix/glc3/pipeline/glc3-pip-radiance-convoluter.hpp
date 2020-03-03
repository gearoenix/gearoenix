#ifndef GEAROENIX_GLC3_PIPELINE_RADIANCE_CONVOLUTER_HPP
#define GEAROENIX_GLC3_PIPELINE_RADIANCE_CONVOLUTER_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../render/pipeline/rnd-pip-radiance-convoluter.hpp"

namespace gearoenix::glc3::engine {
class Engine;
}

namespace gearoenix::glc3::shader {
class RadianceConvoluter;
}

namespace gearoenix::glc3::pipeline {
class RadianceConvoluter final : public render::pipeline::RadianceConvoluter {
private:
    const std::shared_ptr<shader::RadianceConvoluter> shd;
    std::weak_ptr<RadianceConvoluter> self;
    RadianceConvoluter(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;

public:
    [[nodiscard]] static std::shared_ptr<RadianceConvoluter> construct(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    ~RadianceConvoluter() noexcept final;
    [[nodiscard]] render::pipeline::ResourceSet* create_resource_set() const noexcept final;
};
}

#endif
#endif
