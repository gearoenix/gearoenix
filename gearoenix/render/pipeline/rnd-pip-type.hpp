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

    } // namespace pipeline
} // namespace render
} // namespace gearoenix

#endif // RNDPIPTYPE_HPP
