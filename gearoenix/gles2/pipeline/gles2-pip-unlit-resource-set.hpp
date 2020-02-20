#ifndef GEAROENIX_GLES2_PIPELINE_UNLIT_RESOURCE_SET_HPP
#define GEAROENIX_GLES2_PIPELINE_UNLIT_RESOURCE_SET_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../gl/gl-types.hpp"
#include "../../render/pipeline/rnd-pip-unlit-resource-set.hpp"

namespace gearoenix::gles2::shader {
class Unlit;
}

namespace gearoenix::gles2::pipeline {
class Unlit;
class ResourceSet;
class UnlitResourceSet final : public render::pipeline::UnlitResourceSet {
    GX_GET_UCPTR_PRV(gles2::pipeline::ResourceSet, base)
public:
    UnlitResourceSet(const std::shared_ptr<shader::Unlit>& shd, std::shared_ptr<Unlit const> pip) noexcept;
    ~UnlitResourceSet() noexcept final;
    void bind_final(gl::uint& bound_shader_program) const noexcept;
};
}

#endif
#endif
