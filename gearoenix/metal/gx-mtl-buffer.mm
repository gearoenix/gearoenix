#include "gx-mtl-buffer.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#import "../core/gx-cr-allocator.hpp"
#import "gx-mtl-engine.hpp"
#import "gx-mtl-heap.hpp"

gearoenix::metal::UniformBuffer::UniformBuffer(
    std::shared_ptr<const core::Allocator>&& _gpu_range,
    std::array<std::shared_ptr<const core::Allocator>, GEAROENIX_METAL_FRAMES_COUNT>&& _cpu_ranges,
    std::array<std::uint8_t*, GEAROENIX_METAL_FRAMES_COUNT>&& data)
    : gpu_range(std::move(_gpu_range))
    , cpu_ranges(std::move(_cpu_ranges))
    , gpu_offset(static_cast<NSUInteger>(gpu_range->get_offset()))
    , cpu_offsets({
          static_cast<NSUInteger>(cpu_ranges[0]->get_offset()),
          static_cast<NSUInteger>(cpu_ranges[1]->get_offset()),
          static_cast<NSUInteger>(cpu_ranges[2]->get_offset()),
      })
    , size(static_cast<NSUInteger>(gpu_range->get_size()))
    , data(std::move(data))
{
}

gearoenix::metal::UniformBuffer::~UniformBuffer() = default;

gearoenix::metal::UniformBuffer::UniformBuffer(UniformBuffer&& o)
    : gpu_range(o.gpu_range)
    , cpu_ranges(o.cpu_ranges)
    , gpu_offset(o.gpu_offset)
    , cpu_offsets(o.cpu_offsets)
    , size(o.size)
    , data(o.data)
{
}

gearoenix::metal::ArgsBuffer::ArgsBuffer(Engine& e, id<MTLFunction> func, const NSUInteger index, [[maybe_unused]] const std::string& name)
    : encoder([func newArgumentEncoderWithBufferIndex:index])
    , buffer([e.get_device() newBufferWithLength:encoder.encodedLength options:MTLResourceStorageModeShared])
{
#ifdef GEAROENIX_METAL_RESOURCE_NAMING
    encoder.label = [NSString stringWithFormat:@"Gearoenix-ArgsEncoder-%s", name.c_str()];
    buffer.label = [NSString stringWithFormat:@"Gearoenix-ArgsCPUBuffer-%s", name.c_str()];
#endif
    [encoder setArgumentBuffer:buffer
                        offset:0];
}

gearoenix::metal::ArgsBuffer::~ArgsBuffer()
{
    if (nil == encoder)
        return;
    [encoder release];
    [buffer release];
}

gearoenix::metal::ArgsBuffer::ArgsBuffer(ArgsBuffer&& o)
    : encoder(o.encoder)
    , buffer(o.buffer)
{
    o.encoder = nil;
}

gearoenix::metal::BufferManager::BufferManager(Engine& e)
    : uniforms_gpu([e.get_heap_manager()->gpu newBufferWithLength:GEAROENIX_METAL_BUFFER_UNIFORM_MAX_SIZE options:MTLResourceStorageModePrivate])
    , uniforms_cpu([e.get_device() newBufferWithLength:GEAROENIX_METAL_BUFFER_UNIFORM_MAX_SIZE * GEAROENIX_METAL_FRAMES_COUNT options:MTLResourceStorageModeShared])
    , uniforms_gpu_range(core::Allocator::construct(GEAROENIX_METAL_BUFFER_UNIFORM_MAX_SIZE))
    , uniforms_cpu_range(core::Allocator::construct(GEAROENIX_METAL_BUFFER_UNIFORM_MAX_SIZE * GEAROENIX_METAL_FRAMES_COUNT))
    , uniforms_cpu_ranges {
        uniforms_cpu_range->allocate(GEAROENIX_METAL_BUFFER_UNIFORM_MAX_SIZE),
        uniforms_cpu_range->allocate(GEAROENIX_METAL_BUFFER_UNIFORM_MAX_SIZE),
        uniforms_cpu_range->allocate(GEAROENIX_METAL_BUFFER_UNIFORM_MAX_SIZE),
    }
{
#ifdef GEAROENIX_METAL_RESOURCE_NAMING
    uniforms_gpu.label = @"Gearoenix-UniformGPUBuffer";
    uniforms_cpu.label = @"Gearoenix-UniformCPUBuffer";
#endif
}

gearoenix::metal::BufferManager::~BufferManager()
{
    [uniforms_cpu release];
    [uniforms_gpu release];
}

gearoenix::metal::UniformBuffer gearoenix::metal::BufferManager::create_uniform(const NSUInteger unaligned_size)
{
    const auto size = (unaligned_size + 255) & ~255;
    std::array<std::shared_ptr<const core::Allocator>, GEAROENIX_METAL_FRAMES_COUNT> cpu_ranges {
        uniforms_cpu_ranges[0]->allocate(size),
        uniforms_cpu_ranges[1]->allocate(size),
        uniforms_cpu_ranges[2]->allocate(size),
    };
    auto base_ptr = reinterpret_cast<std::uint8_t*>(uniforms_cpu.contents);
    std::array<std::uint8_t*, GEAROENIX_METAL_FRAMES_COUNT> data {
        cpu_ranges[0]->get_offset() + base_ptr,
        cpu_ranges[1]->get_offset() + base_ptr,
        cpu_ranges[2]->get_offset() + base_ptr,
    };
    return UniformBuffer(uniforms_gpu_range->allocate(size), std::move(cpu_ranges), std::move(data));
}

void gearoenix::metal::BufferManager::update(id<MTLBlitCommandEncoder> blit, std::size_t frame_number)
{
    [blit
           copyFromBuffer:uniforms_cpu
             sourceOffset:static_cast<NSUInteger>(uniforms_cpu_ranges[frame_number]->get_offset())
                 toBuffer:uniforms_gpu
        destinationOffset:0
                     size:uniforms_gpu.length];
}
#endif
