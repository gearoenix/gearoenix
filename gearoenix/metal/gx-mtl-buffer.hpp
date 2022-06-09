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
#import <memory>
#import <string>

namespace gearoenix::core {
struct Allocator;
}

namespace gearoenix::metal {
struct Engine;
struct UniformBuffer final {
    const std::shared_ptr<const core::Allocator> gpu_range;
    const std::array<std::shared_ptr<const core::Allocator>, GEAROENIX_METAL_FRAMES_COUNT> cpu_ranges;
    const NSUInteger gpu_offset;
    const std::array<NSUInteger, GEAROENIX_METAL_FRAMES_COUNT> cpu_offsets;
    const NSUInteger size;
    const std::array<std::uint8_t*, GEAROENIX_METAL_FRAMES_COUNT> data;

    UniformBuffer(
        std::shared_ptr<const core::Allocator>&& gpu_range,
        std::array<std::shared_ptr<const core::Allocator>, GEAROENIX_METAL_FRAMES_COUNT>&& cpu_ranges,
        std::array<std::uint8_t*, GEAROENIX_METAL_FRAMES_COUNT>&& data) noexcept;
    ~UniformBuffer() noexcept;
    UniformBuffer(UniformBuffer&&) noexcept;
};

struct ArgsBuffer final {
    id<MTLArgumentEncoder> encoder;
    id<MTLBuffer> buffer;

    ArgsBuffer(Engine& e, id<MTLFunction> func, NSUInteger index, const std::string& name) noexcept;
    ~ArgsBuffer() noexcept;
    ArgsBuffer(ArgsBuffer&&) noexcept;
};

struct BufferManager final {
    const id<MTLBuffer> uniforms_gpu;
    const id<MTLBuffer> uniforms_cpu;
    const std::shared_ptr<core::Allocator> uniforms_gpu_range;
    const std::shared_ptr<core::Allocator> uniforms_cpu_range;
    const std::array<std::shared_ptr<core::Allocator>, GEAROENIX_METAL_FRAMES_COUNT> uniforms_cpu_ranges;

    BufferManager(Engine& e) noexcept;
    ~BufferManager() noexcept;
    BufferManager(const BufferManager&) = delete;
    [[nodiscard]] UniformBuffer create_uniform(NSUInteger size) noexcept;
    void update(id<MTLBlitCommandEncoder> blit, std::size_t frame_number) noexcept;
};
}

#endif
#endif
