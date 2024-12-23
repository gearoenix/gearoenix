#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#import <Metal/MTLHeap.h>

namespace gearoenix::metal {
struct Engine;
struct HeapManager final {
    const id<MTLHeap> gpu;

    HeapManager(Engine& e);
    ~HeapManager();
};
}

#endif