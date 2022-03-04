#ifndef GEAROENIX_METAL_BUFFER_HPP
#define GEAROENIX_METAL_BUFFER_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#import "shaders/gx-mtl-shd-common.hpp"
#import <Metal/MTLArgumentEncoder.h>
#import <Metal/MTLBlitCommandEncoder.h>
#import <Metal/MTLBuffer.h>
#import <Metal/MTLFunctionHandle.h>
#import <array>
#import <string>

namespace gearoenix::metal {
struct Engine;
struct UniformBuffer final {
    id<MTLBuffer> gpu;
    std::array<id<MTLBuffer>, GEAROENIX_METAL_FRAMES_COUNT> cpu;
    
    UniformBuffer(Engine& e, NSUInteger size, const std::string& name) noexcept;
    ~UniformBuffer() noexcept;
    UniformBuffer(UniformBuffer&&) noexcept;
    [[nodiscard]] std::uint8_t* data(std::size_t index) noexcept;
    void update(id<MTLBlitCommandEncoder> blit, std::size_t frame_number) noexcept;
};

struct ArgsBuffer final {
    id<MTLArgumentEncoder> encoder;
    id<MTLBuffer> buffer;
    
    ArgsBuffer(Engine& e, id<MTLFunction> func, NSUInteger index, const std::string& name) noexcept;
    ~ArgsBuffer() noexcept;
    ArgsBuffer(ArgsBuffer&&) noexcept;
};
}

#endif
#endif
