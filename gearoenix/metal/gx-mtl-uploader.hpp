#ifndef GEAROENIX_METAL_UPLOADER_HPP
#define GEAROENIX_METAL_UPLOADER_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#import "../core/sync/gx-cr-sync-end-caller.hpp"
#import "../core/sync/gx-cr-sync-work-waiter.hpp"
#import <Metal/MTLBuffer.h>
#import <Metal/MTLTexture.h>
#import <Metal/MTLCommandQueue.h>

namespace gearoenix::metal {
struct Engine;
struct Uploader final {
private:
    Engine& e;
    core::sync::WorkWaiter uploader;
    const id<MTLCommandQueue> queue;

    void upload(id<MTLBuffer> destination, const void* data, const std::size_t size, core::sync::EndCallerIgnored&& c) noexcept;
    
public:
    Uploader(Engine& e) noexcept;
    ~Uploader() noexcept;
    
    template<typename T>
    void upload(id<MTLBuffer> destination, const std::vector<T>& data, core::sync::EndCallerIgnored&& c) noexcept {
        upload(destination, data.data(), data.size() * sizeof(T), std::move(c));
    }
    
    void upload(
                id<MTLTexture> destination,
                MTLTextureDescriptor* texture_descriptor,
                std::vector<std::vector<std::uint8_t>>&& pixels,
                core::sync::EndCallerIgnored&& c) noexcept;
};
}

#endif
#endif
