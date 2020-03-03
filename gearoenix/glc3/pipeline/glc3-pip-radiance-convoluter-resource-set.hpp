#ifndef GEAROENIX_GLC3_PIPELINE_RADIANCE_CONVOLUTER_RESOURCE_SET_HPP
#define GEAROENIX_GLC3_PIPELINE_RADIANCE_CONVOLUTER_RESOURCE_SET_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../gl/gl-types.hpp"
#include "../../render/pipeline/rnd-pip-radiance-convoluter-resource-set.hpp"

namespace gearoenix::glc3::shader {
class RadianceConvoluter;
}

namespace gearoenix::glc3::pipeline {
class RadianceConvoluter;
class ResourceSet;
class RadianceConvoluterResourceSet final : public render::pipeline::RadianceConvoluterResourceSet {
    GX_GET_UCPTR_PRV(glc3::pipeline::ResourceSet, base)
public:
    RadianceConvoluterResourceSet(const std::shared_ptr<shader::RadianceConvoluter>& shd, std::shared_ptr<RadianceConvoluter const> pip) noexcept;
    ~RadianceConvoluterResourceSet() noexcept final;
    void bind_final(gl::uint& bound_shader_program) const noexcept;
};
}

#endif
#endif
