#ifndef GEAROEMIX_RENDER_PIPELINE_SHADOW_MAPPER_HPP
#define GEAROEMIX_RENDER_PIPELINE_SHADOW_MAPPER_HPP
#include "rnd-pip-pipeline.hpp"
namespace gearoenix {
namespace render {
    namespace pipeline {
        class ShadowMapper : public Pipeline {
        protected:
            ShadowMapper(const std::shared_ptr<engine::Engine> e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call);

        public:
            virtual ~ShadowMapper();
        };
    }
}
}
#endif
