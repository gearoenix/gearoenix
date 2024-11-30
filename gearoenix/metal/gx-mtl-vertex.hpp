#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#import <Metal/MTLVertexDescriptor.h>

namespace gearoenix::metal {
struct VertexDescriptions final {
    constexpr static int PbrAttrIndexPosition = 0;
    constexpr static int PbrAttrIndexNormal = 1;
    constexpr static int PbrAttrIndexTangent = 2;
    constexpr static int PbrAttrIndexUV = 3;
    constexpr static int PbrLayoutIndex = 0;

    MTLVertexDescriptor* const pbr;

    VertexDescriptions();
};
}

#endif