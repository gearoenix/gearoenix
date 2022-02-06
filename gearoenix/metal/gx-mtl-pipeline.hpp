#ifndef GEAROENIX_METAL_PIPELINE_HPP
#define GEAROENIX_METAL_PIPELINE_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#import "../core/macro/gx-cr-mcr-getter-setter.hpp"
#import <Metal/MTLLibrary.h>

namespace gearoenix::metal {
struct Engine;
struct PipelineManager final {
    GX_GET_VAL_PRV(id<MTLLibrary>, all_shaders_lib, nil)
    
    PipelineManager(Engine& e) noexcept;
};
}

#endif
#endif
