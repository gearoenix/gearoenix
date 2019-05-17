#ifndef GEAROENIX_GLES2_PIPELINE_SHADOW_MAPPER_RESOURCE_SET_HPP
#define GEAROENIX_GLES2_PIPELINE_SHADOW_MAPPER_RESOURCE_SET_HPP
#include "../../render/pipeline/rnd-pip-shadow-mapper-resource-set.hpp"
#include "gles2-pip-resource-set.hpp"
namespace gearoenix {
namespace gles2 {
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
}
#endif
