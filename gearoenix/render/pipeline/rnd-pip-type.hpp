#ifndef GEAROENIX_RENDER_PIPELINE_TYPE_HPP
#define GEAROENIX_RENDER_PIPELINE_TYPE_HPP
#include "../../core/cr-types.hpp"
namespace gearoenix {
namespace render {
    namespace pipeline {
        class Type {
        public:
            typedef enum : core::Id {
                DeferredPbr = 1,
                ForwardPbr = 2,
                /// it has cone light shadow map
                ForwardPbrConeShadow = 3,
                /// it has directional light shadow map (no cascaded)
                ForwardPbrDirectionalShadow = 4,
                /// it has point light shadow maps
                ForwardPbrPointShadow = 5,
                GBufferFiller = 6,
                LightBlooming = 7,
                ShadowAccumulatorCone = 8,
                ShadowAccumulatorDirectional = 9,
                ShadowAccumulatorPoint = 10,
                ShadowMapper = 11,
                SSAO = 12,
                SSR = 13,
                Unlit = 14,
            } Id;
        };
    }
}
}
#endif
