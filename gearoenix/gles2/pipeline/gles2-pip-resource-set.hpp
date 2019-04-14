#ifndef GEAROEMIX_GLES2_PIPELINE_RESOURCE_SET_HPP
#define GEAROEMIX_GLES2_PIPELINE_RESOURCE_SET_HPP
#include "../../gl/gl-types.hpp"
#include <memory>
namespace gearoenix {
namespace gles2 {
    namespace shader {
        class Shader;
    }
    namespace pipeline {
        class ResourceSet {
        protected:
            const std::shared_ptr<shader::Shader> shd;

        public:
            ResourceSet(const std::shared_ptr<shader::Shader>& shd);
            virtual void bind(gl::uint& bound_shader_program) const;
        };
    }
}
}
#endif
