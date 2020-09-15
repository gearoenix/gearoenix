#ifndef GEAROENIX_GLC3_PIPELINE_UNLIT_RESOURCE_SET_HPP
#define GEAROENIX_GLC3_PIPELINE_UNLIT_RESOURCE_SET_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../gl/gx-gl-types.hpp"
#include "../../render/pipeline/gx-rnd-pip-unlit-resource-set.hpp"

namespace gearoenix::glc3::shader {
class Unlit;
}

namespace gearoenix::glc3::pipeline {
class Unlit;
class ResourceSet;
class UnlitResourceSet final : public render::pipeline::UnlitResourceSet {
    GX_GET_UCPTR_PRV(glc3::pipeline::ResourceSet, base)
public:
    UnlitResourceSet(const std::shared_ptr<shader::Unlit>& shd, std::shared_ptr<Unlit const> pip) noexcept;
    ~UnlitResourceSet() noexcept final;
    void bind_final(gl::uint& bound_shader_program) const noexcept;
};
}

#endif
#endif
