#ifndef GEAROENIX_METAL_PIPELINE_HPP
#define GEAROENIX_METAL_PIPELINE_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#import "../core/macro/gx-cr-mcr-getter-setter.hpp"
#import "gx-mtl-vertex.hpp"
#import <Metal/MTLDepthStencil.h>
#import <Metal/MTLFunctionHandle.h>
#import <Metal/MTLLibrary.h>

namespace gearoenix::metal {
struct Engine;
struct PipelineManager final {
    GX_GET_VAL_PRV(id<MTLLibrary>, all_shaders_lib, nil)
    GX_GET_VAL_PRV(id<MTLFunction>, gbuffers_filler_vertex_shader, nil)
    GX_GET_VAL_PRV(id<MTLFunction>, gbuffers_filler_fragment_shader, nil)
    GX_GET_VAL_PRV(id<MTLRenderPipelineState>, gbuffers_filler_ps, nil)
    GX_GET_VAL_PRV(id<MTLDepthStencilState>, depth_state, nil)
    GX_GET_CREF_PRV(VertexDescriptions, vertex_descriptions)

    PipelineManager(Engine& e);
};
}

#endif
#endif
