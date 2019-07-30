#ifndef GEAROENIX_GLES2_PIPELINE_FORWARD_PBR_RESOURCE_SET_HPP
#define GEAROENIX_GLES2_PIPELINE_FORWARD_PBR_RESOURCE_SET_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../render/pipeline/rnd-pip-forward-pbr-resource-set.hpp"
#include "gles2-pip-resource-set.hpp"
namespace gearoenix {
namespace gles2 {
    namespace shader {
        class ForwardPbr;
    }
    namespace pipeline {
        class ForwardPbrResourceSet : public ResourceSet, public render::pipeline::ForwardPbrResourceSet {
        public:
            ForwardPbrResourceSet(const std::shared_ptr<shader::ForwardPbr>& shd);
            void bind(gl::uint& bound_shader_program) const override final;
        };
    }
}
}
#endif
#endif
