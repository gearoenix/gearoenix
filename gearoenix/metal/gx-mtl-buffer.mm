#include "gx-mtl-buffer.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#import "gx-mtl-engine.hpp"
#import "gx-mtl-heap.hpp"

gearoenix::metal::UniformBuffer::UniformBuffer(Engine& e, const NSUInteger size, [[maybe_unused]] const std::string& name) noexcept
    : gpu([e.get_heap_manager()->gpu newBufferWithLength:size options:MTLResourceStorageModePrivate])
    , cpu{
        [e.get_device() newBufferWithLength:size options:MTLResourceStorageModeShared],
        [e.get_device() newBufferWithLength:size options:MTLResourceStorageModeShared],
        [e.get_device() newBufferWithLength:size options:MTLResourceStorageModeShared]
    }
{
#ifdef GEAROENIX_METAL_RESOURCE_NAMING
    gpu.label = [NSString stringWithFormat:@"Gearoenix-UniformGPUBuffer-%s", name.c_str()];
    std::uint32_t index = 0;
    for(auto& b: cpu) {
        b.label = [NSString stringWithFormat:@"Gearoenix-UniformCPUBuffer-%s-%u", name.c_str(), index];
        ++index;
    }
#endif
}

gearoenix::metal::UniformBuffer::~UniformBuffer() noexcept
{
    if(nil != gpu)
        [gpu release];
    for(auto& b: cpu)
        if(nil != b)
            [b release];
}

gearoenix::metal::UniformBuffer::UniformBuffer(UniformBuffer&& o) noexcept
{
    gpu = o.gpu;
    o.gpu = nil;
    for(std::size_t i = 0; i < cpu.size(); ++i)
    {
        cpu[i] = o.cpu[i];
        o.cpu[i] = nil;
    }
}

std::uint8_t* gearoenix::metal::UniformBuffer::data(const std::size_t index) noexcept
{
    return reinterpret_cast<std::uint8_t*>(cpu[index].contents);
}

void gearoenix::metal::UniformBuffer::update(const id<MTLBlitCommandEncoder> blit, const std::size_t frame_number) noexcept
{
    [blit copyFromBuffer:cpu[frame_number] sourceOffset:0 toBuffer:gpu destinationOffset:0 size:gpu.length];
}

gearoenix::metal::ArgsBuffer::ArgsBuffer(Engine& e, id<MTLFunction> func, const NSUInteger index, [[maybe_unused]] const std::string& name) noexcept
    : encoder([func newArgumentEncoderWithBufferIndex:index])
    , buffer([e.get_device() newBufferWithLength:encoder.encodedLength options:MTLResourceStorageModeShared])
{
#ifdef GEAROENIX_METAL_RESOURCE_NAMING
    encoder.label = [NSString stringWithFormat:@"Gearoenix-ArgsEncoder-%s", name.c_str()];
    buffer.label = [NSString stringWithFormat:@"Gearoenix-ArgsCPUBuffer-%s", name.c_str()];
#endif
    [encoder setArgumentBuffer:buffer offset:0];
}

gearoenix::metal::ArgsBuffer::~ArgsBuffer() noexcept
{
    if(nil == encoder) return;
    [encoder release];
    [buffer release];
}

gearoenix::metal::ArgsBuffer::ArgsBuffer(ArgsBuffer&& o) noexcept
    : encoder(o.encoder)
    , buffer(o.buffer)
{
    o.encoder = nil;
}

#endif
