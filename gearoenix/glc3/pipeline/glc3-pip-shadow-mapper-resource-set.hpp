#ifndef GEAROENIX_GLC3_PIPELINE_SHADOW_MAPPER_RESOURCE_SET_HPP
#define GEAROENIX_GLC3_PIPELINE_SHADOW_MAPPER_RESOURCE_SET_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../render/pipeline/rnd-pip-shadow-mapper-resource-set.hpp"
#include "glc3-pip-resource-set.hpp"
namespace gearoenix::glc3 {
namespace shader {
    class ShadowMapper;
}
namespace pipeline {
    class ShadowMapperResourceSet : public ResourceSet, public render::pipeline::ShadowMapperResourceSet {
    public:
        ShadowMapperResourceSet(const std::shared_ptr<shader::ShadowMapper>& shd);
        void bind(gl::uint& bound_shader_program) const override final;
    };
}
}
#endif
#endif
