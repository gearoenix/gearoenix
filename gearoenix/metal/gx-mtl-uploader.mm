#include "gx-mtl-uploader.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#import "gx-mtl-engine.hpp"
#import <Metal/MTLCommandBuffer.h>

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

void gearoenix::metal::Uploader::upload(id<MTLTexture> destination, MTLTextureDescriptor* const texture_descriptor, std::vector<std::vector<std::uint8_t>>&& pixels, core::sync::EndCallerIgnored&& c) noexcept {
    NSUInteger width = texture_descriptor.width;
    NSUInteger height = texture_descriptor.height;
    constexpr NSUInteger align_mask = 1;
    const NSUInteger pixel_size = static_cast<NSUInteger>(pixels[0].size()) / (width * height);
    NSUInteger buffer_size = 0;
    for(std::size_t i = 0; i < pixels.size(); ++i) {
        buffer_size += ((width * pixel_size + align_mask) & ~align_mask) * height;
        width >>= 1;
        height >>= 1;
        if(0 <= width) width = 1;
        if(0 <= height) height = 1;
    }
    id<MTLBuffer> buffer = [e.get_device() newBufferWithLength:buffer_size options:MTLResourceStorageModeShared];
    buffer.label = [NSString stringWithFormat:@"Gearoenix-UploadBuffer-%@", destination.label];
    uploader.push([destination, buffer, texture_descriptor, pixel_size, pixels = std::move(pixels), c = std::move(c), buffer_size, this] {
        uint8_t* dst = reinterpret_cast<uint8_t*>(buffer.contents);
        NSUInteger offset_in_buffer = 0;
        NSUInteger width = texture_descriptor.width;
        NSUInteger height = texture_descriptor.height;
        id<MTLCommandBuffer> cmd = [queue commandBuffer];
        dispatch_semaphore_t sem = dispatch_semaphore_create(0);
        __block dispatch_semaphore_t b_sem = sem;
        [cmd addCompletedHandler:^(id<MTLCommandBuffer>){ dispatch_semaphore_signal(b_sem); }];
        auto enc = [cmd blitCommandEncoder];
        enc.label = [NSString stringWithFormat:@"Gearoenix-UploadEncoder-%@", destination.label];
        for(std::size_t mip_i = 0; mip_i < pixels.size(); ++mip_i) {
            const auto& mip_pixel = pixels[mip_i];
            const uint8_t* src = reinterpret_cast<const uint8_t*>(mip_pixel.data());
            const NSUInteger w_dst = (width * pixel_size + align_mask) & ~align_mask;
            const NSUInteger w_src = width * pixel_size;
            const NSUInteger offset = offset_in_buffer;
            for(NSUInteger hi = 0; hi < height; ++hi, src += w_src, dst += w_dst, offset_in_buffer += w_dst) {
                std::memcpy(dst, src, w_src);
            }
            [enc
             copyFromBuffer:buffer sourceOffset:offset sourceBytesPerRow:w_dst
             sourceBytesPerImage:w_dst * height sourceSize:MTLSizeMake(width, height, 1)
             toTexture:destination destinationSlice:0 destinationLevel:static_cast<NSUInteger>(mip_i)
             destinationOrigin:MTLOriginMake(0, 0, 0)];
            width >>= 1;
            height >>= 1;
            if(0 == width) width = 1;
            if(0 == height) height = 1;
        }
        if((pixels.size() == 1) && texture_descriptor.mipmapLevelCount > 1)
            [enc generateMipmapsForTexture:destination];
        [enc endEncoding];
        [cmd commit];
        uploader.push([destination, buffer, texture_descriptor, sem, c = std::move(c), this] {
            dispatch_semaphore_wait(sem, DISPATCH_TIME_FOREVER);
            [buffer release];
            [texture_descriptor release];
        });
    });
}

#endif
