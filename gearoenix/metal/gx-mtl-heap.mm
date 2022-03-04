#include "gx-mtl-heap.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#import "gx-mtl-engine.hpp"

namespace
{
static MTLHeapDescriptor* create_heap_descriptor(const NSUInteger size, const MTLStorageMode storage_mode ) noexcept {
    MTLHeapDescriptor * const desc = [MTLHeapDescriptor new];
    desc.storageMode = storage_mode;
    desc.size = size;
    return desc;
}
}

gearoenix::metal::HeapManager::HeapManager(Engine& e) noexcept
    : gpu([e.get_device() newHeapWithDescriptor:create_heap_descriptor(512 * 1024 * 1024, MTLStorageModePrivate)])
{}

gearoenix::metal::HeapManager::~HeapManager() noexcept {
    [gpu release];
}

#endif
