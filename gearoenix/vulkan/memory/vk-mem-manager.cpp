#include "vk-mem-manager.hpp"
#ifdef USE_VULKAN
#include "../../core/cr-static.hpp"
#include "../buffer/vk-buf-buffer.hpp"
#include "../device/vk-dev-logical.hpp"
#include "../device/vk-dev-physical.hpp"
#include "../vk-check.hpp"
#include "../vk-instance.hpp"
#include "vk-mem-memory.hpp"
#include "vk-mem-sub-memory.hpp"
gearoenix::render::memory::Manager::Manager(device::Logical* logical_device, unsigned int size, const Place& place)
    : Gc(size)
    , align(logical_device->get_physical_device()->get_max_memory_alignment())
    , comalign(align - 1)
    , decomalign(~comalign)
{
    mem_reqs.alignment = logical_device->get_physical_device()->get_max_memory_alignment();
    mem_reqs.memoryTypeBits = 0;
    mem_reqs.memoryTypeBits |= buffer::Buffer::get_memory_type_bits(logical_device, place == GPU_LOCAL);
    mem_reqs.size = size;
    // GXTODO
    uint32_t mem_place;
    switch (place) {
    case GPU_LOCAL:
        mem_place = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        break;
    case CPU_COHERENT:
        mem_place = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        break;
    default:
        LOGF("Unexpected");
    }
    mem = new Memory(logical_device, mem_reqs, mem_place);
}

gearoenix::render::memory::Manager::~Manager()
{
    delete mem;
}

gearoenix::render::memory::Memory* gearoenix::render::memory::Manager::get_memory()
{
    return mem;
}

const gearoenix::render::memory::Memory* gearoenix::render::memory::Manager::get_memory() const
{
    return mem;
}

gearoenix::render::memory::SubMemory* gearoenix::render::memory::Manager::create_submemory(unsigned int size)
{
    if ((comalign & size) != 0) {
        size = (decomalign & size) + align;
    }
    SubMemory* submem = new SubMemory(size, mem);
    allocate(submem);
    return submem;
}
#endif
