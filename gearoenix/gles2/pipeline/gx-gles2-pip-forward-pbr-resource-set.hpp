#ifndef GEAROENIX_GLES2_PIPELINE_FORWARD_PBR_RESOURCE_SET_HPP
#define GEAROENIX_GLES2_PIPELINE_FORWARD_PBR_RESOURCE_SET_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../gl/gx-gl-types.hpp"
#include "../../render/pipeline/gx-rnd-pip-forward-pbr-resource-set.hpp"

namespace gearoenix::gles2::shader {
class ForwardPbr;
}

namespace gearoenix::gles2::pipeline {
class ForwardPbr;
class ResourceSet;
class ForwardPbrResourceSet final : public render::pipeline::ForwardPbrResourceSet {
    GX_GET_UCPTR_PRV(gles2::pipeline::ResourceSet, base)
public:
    ForwardPbrResourceSet(const std::shared_ptr<shader::ForwardPbr>& shd, std::shared_ptr<ForwardPbr const> pip) noexcept;
    ~ForwardPbrResourceSet() noexcept final;
    void bind_final(gl::uint& bound_shader_program) const noexcept;
};
}
#endif
#endif
