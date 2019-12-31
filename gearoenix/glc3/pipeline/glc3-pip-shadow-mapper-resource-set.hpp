#ifndef GEAROENIX_GLC3_PIPELINE_SHADOW_MAPPER_RESOURCE_SET_HPP
#define GEAROENIX_GLC3_PIPELINE_SHADOW_MAPPER_RESOURCE_SET_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../render/pipeline/rnd-pip-shadow-mapper-resource-set.hpp"
#include "glc3-pip-resource-set.hpp"

namespace gearoenix::glc3::shader {
class ShadowMapper;
}

namespace gearoenix::glc3::pipeline {
class ShadowMapperResourceSet : public ResourceSet, public render::pipeline::ShadowMapperResourceSet {
public:
    explicit ShadowMapperResourceSet(const std::shared_ptr<shader::ShadowMapper>& shd) noexcept;
    void bind(gl::uint& bound_shader_program) const noexcept final;
};
}

#endif
#endif
