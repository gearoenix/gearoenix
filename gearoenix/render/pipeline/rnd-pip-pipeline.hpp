#ifndef GEAROENIX_RENDER_PIPELINE_PIPELINE_HPP
#define GEAROENIX_RENDER_PIPELINE_PIPELINE_HPP
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include <memory>
namespace gearoenix {
namespace render {
    class Engine;
    namespace shader {
        class Shader;
    }
    namespace pipeline {
		class Resource;
		class ResourceSet;
		class PipelineType {
		public:
			typedef enum : core::Id {
				DeferredPbr = 1,
				GBufferFiller = 2,
				ForwardPbr = 3,
				/// it has directional light shadow map (no cascaded)
				ForwardPbrDirectionalShadow = 4,
				/// it has point light shadow map
				ForwardPbrPointShadow = 5,
				/// it has cone light shadow map
				ForwardPbrConeShadow = 6,
				ShadowAccumulatorDirectional = 7,
				ShadowAccumulatorPoint = 8,
				ShadowAccumulatorCone = 9,
				ShadowMapper = 10,
				LightBlooming = 11,
				SSAO = 12,
				SSR = 13,
				Unlit = 14,
			} Id;
		};
        class Pipeline {
        protected:
            Engine* eng;
			const PipelineType::Id pipeline_type;

        public:
            Pipeline(PipelineType::Id pipeline_type, Engine* eng, core::sync::EndCaller<core::sync::EndCallerIgnore> call);
            virtual ~Pipeline();
			PipelineType::Id get_pipeline_type_id() const;
			virtual ResourceSet *create_resource_set() const = 0;
        };
    }
}
}
#endif // GEAROENIX_RENDER_PIPELINE_PIPELINE_HPP
