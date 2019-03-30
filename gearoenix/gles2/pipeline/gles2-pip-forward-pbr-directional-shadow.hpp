#ifndef GEAROEMIX_GLES2_PIPELINE_FORWARD_PBR_DIRECTIONAL_SHADOW_HPP
#define GEAROEMIX_GLES2_PIPELINE_FORWARD_PBR_DIRECTIONAL_SHADOW_HPP
#include "gles2-pip-pipeline.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
namespace gearoenix {
	namespace gles2 {
		namespace shader {
			class ForwardPbrDirectionalShadow;
		}
		namespace pipeline {
			class ForwardPbrDirectionalShadow : public Pipeline {
			private:
				const std::shared_ptr<shader::ForwardPbrDirectionalShadow> shd;
			public:
				ForwardPbrDirectionalShadow(const std::shared_ptr<engine::Engine> &e, const core::sync::EndCaller<core::sync::EndCallerIgnore> &c);
				render::pipeline::ResourceSet* create_resource_set() const override final;
			};
		}
	}
}
#endif
