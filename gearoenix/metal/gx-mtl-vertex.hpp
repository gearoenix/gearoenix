#ifndef GEAROENIX_METAL_VERTEX_HPP
#define GEAROENIX_METAL_VERTEX_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#import <Metal/MTLVertexDescriptor.h>

namespace gearoenix::metal {
struct VertexDescriptions final {
    static constexpr int PbrAttrIndexPosition = 0;
    static constexpr int PbrAttrIndexNormal = 1;
    static constexpr int PbrAttrIndexTangent = 2;
    static constexpr int PbrAttrIndexUV = 3;
    static constexpr int PbrLayoutIndex = 0;

    MTLVertexDescriptor* const pbr;

    VertexDescriptions();
};
}

#endif
#endif
