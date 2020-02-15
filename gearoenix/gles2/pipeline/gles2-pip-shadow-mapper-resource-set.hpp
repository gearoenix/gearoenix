#ifndef GEAROENIX_GLES2_PIPELINE_SHADOW_MAPPER_RESOURCE_SET_HPP
#define GEAROENIX_GLES2_PIPELINE_SHADOW_MAPPER_RESOURCE_SET_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../gl/gl-types.hpp"
#include "../../render/pipeline/rnd-pip-shadow-mapper-resource-set.hpp"

namespace gearoenix::gles2::shader {
class ShadowMapper;
}

namespace gearoenix::gles2::pipeline {
class ShadowMapper;
class ResourceSet;
class ShadowMapperResourceSet : public render::pipeline::ShadowMapperResourceSet {
    GX_GET_UCPTR_PRV(gles2::pipeline::ResourceSet, base)
public:
    ShadowMapperResourceSet(const std::shared_ptr<shader::ShadowMapper>& shd, std::shared_ptr<ShadowMapper const> pip) noexcept;
    ~ShadowMapperResourceSet() noexcept final;
    void bind_final(gl::uint& bound_shader_program) const noexcept;
};
}
#endif
#endif
