#include "gx-mtl-uploader.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#import "gx-mtl-engine.hpp"

void gearoenix::metal::Uploader::upload(id<MTLBuffer> destination, const void* data, const std::size_t size, core::sync::EndCallerIgnored&& c) noexcept {
    auto source = [e.get_device() newBufferWithBytes:data length:static_cast<NSUInteger>(size) options:MTLResourceStorageModeShared];
    source.label = [NSString stringWithFormat:@"Gearoenix-UploadBuffer-%@", destination.label];
    uploader.push([destination, source, size, c = std::move(c), this] {
        auto cmd = [queue commandBuffer];
        auto sem = dispatch_semaphore_create(0);
        __block auto b_sem = sem;
        [cmd addCompletedHandler:^(id<MTLCommandBuffer>){ dispatch_semaphore_signal(b_sem); }];
        auto enc = [cmd blitCommandEncoder];
        enc.label = [NSString stringWithFormat:@"Gearoenix-UploadEncoder-%@", destination.label];
        [enc copyFromBuffer:source sourceOffset:0 toBuffer:destination destinationOffset:0 size:static_cast<NSUInteger>(size)];
        [enc endEncoding];
        [cmd commit];
        uploader.push([destination, source, sem, c = std::move(c), this] {
            dispatch_semaphore_wait(sem, DISPATCH_TIME_FOREVER);
            [source release];
        });
    });
}

gearoenix::metal::Uploader::Uploader(Engine& e) noexcept: e(e), queue([e.get_device() newCommandQueue]) {}

gearoenix::metal::Uploader::~Uploader() noexcept = default;

#endif
