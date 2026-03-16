#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#import "../core/sync/gx-cr-job-end-caller.hpp"
#import "../core/sync/gx-cr-sync-work-waiter.hpp"
#import <Metal/MTLBuffer.h>
#import <Metal/MTLCommandQueue.h>
#import <Metal/MTLTexture.h>

namespace gearoenix::metal {
struct Engine;
struct Uploader final {
private:
    Engine& e;
    core::sync::WorkWaiter uploader;
    const id<MTLCommandQueue> queue;

    void upload(id<MTLBuffer> destination, const void* data, const std::uint32_t size, core::job::EndCaller&& c);

public:
    Uploader(Engine& e);
    ~Uploader();

    template <typename T>
    void upload(id<MTLBuffer> destination, const std::vector<T>& data, core::job::EndCaller&& c) { upload(destination, data.data(), data.size() * sizeof(T), std::move(c)); }

    void upload(id<MTLTexture> destination, MTLTextureDescriptor* texture_descriptor, std::vector<std::vector<std::uint8_t>>&& pixels, core::job::EndCaller&& c);
};
}

#endif