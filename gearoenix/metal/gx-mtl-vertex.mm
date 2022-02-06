#include "gx-mtl-vertex.hpp"
#ifdef GX_RENDER_METAL_ENABLED

gearoenix::metal::VertexDescriptions::VertexDescriptions() noexcept
    : pbr([[MTLVertexDescriptor alloc] init])
{
    pbr.attributes[PbrAttrIndexPosition].format = MTLVertexFormatFloat3;
    pbr.attributes[PbrAttrIndexPosition].offset = 0;
    pbr.attributes[PbrAttrIndexPosition].bufferIndex = 0;
    pbr.attributes[PbrAttrIndexNormal].format = MTLVertexFormatFloat3;
    pbr.attributes[PbrAttrIndexNormal].offset = 12;
    pbr.attributes[PbrAttrIndexNormal].bufferIndex = 0;
    pbr.attributes[PbrAttrIndexTangent].format = MTLVertexFormatFloat4;
    pbr.attributes[PbrAttrIndexTangent].offset = 24;
    pbr.attributes[PbrAttrIndexTangent].bufferIndex = 0;
    pbr.attributes[PbrAttrIndexUV].format = MTLVertexFormatFloat2;
    pbr.attributes[PbrAttrIndexUV].offset = 40;
    pbr.attributes[PbrAttrIndexUV].bufferIndex = 0;
    pbr.layouts[PbrLayoutIndex].stride = 48;
    pbr.layouts[PbrLayoutIndex].stepRate = 1;
    pbr.layouts[PbrLayoutIndex].stepFunction = MTLVertexStepFunctionPerVertex;
}

#endif
