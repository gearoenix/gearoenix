#ifndef GEAROENIX_RENDER_PIPELINE_FORWARD_PBR_DIRECTIONAL_HPP
#define GEAROENIX_RENDER_PIPELINE_FORWARD_PBR_DIRECTIONAL_HPP
#include "rnd-pip-pipeline.hpp"
namespace gearoenix {
namespace render {
    namespace pipeline {
        class ForwardPbrDirectionalShadow : public Pipeline {
        protected:
            ForwardPbrDirectionalShadow(const std::shared_ptr<engine::Engine> e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call);

        public:
            virtual ~ForwardPbrDirectionalShadow();
        };
    }
}
}
#endif
