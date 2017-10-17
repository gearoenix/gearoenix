#include "vk-buf-manager.hpp"
#include "../device/vk-dev-logical.hpp"
#include "../device/vk-dev-physical.hpp"
#include "../memory/vk-mem-manager.hpp"
#include "../memory/vk-mem-memory.hpp"
#include "../vk-linker.hpp"
#include "vk-buf-buffer.hpp"
#include "vk-buf-sub-buffer.hpp"

gearoenix::render::buffer::Manager::Manager(memory::Manager* mem_mgr, unsigned int size, Usage usage)
    : Gc(size)
    , mem_mgr(mem_mgr)
    , usage(usage)
{
    const VkPhysicalDeviceProperties& pdevpro = mem_mgr->get_memory()->get_logical_device()->get_physical_device()->get_properties();
    align = pdevpro.limits.minUniformBufferOffsetAlignment;
    coalign = align - 1;
    decoalign = ~coalign;
    uint32_t buf_usage = 0;
    if ((usage & VERTEX) == VERTEX) {
        buf_usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    }
    if ((usage & UNIFORM) == UNIFORM) {
        buf_usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    }
    if ((usage & INDEX) == INDEX) {
        buf_usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    }
    buff = new Buffer(mem_mgr, size, buf_usage);
}

gearoenix::render::buffer::Manager::Manager(unsigned int size, Manager* parent)
    : Gc(size, parent)
    , mem_mgr(parent->mem_mgr)
    , buff(parent->buff)
    , usage(parent->usage)
    , align(parent->align)
    , coalign(parent->coalign)
    , decoalign(parent->decoalign)
    , parent(parent)
{
}

gearoenix::render::buffer::Manager::~Manager()
{
    if (nullptr != parent)
        delete buff;
}

const gearoenix::render::buffer::Buffer* gearoenix::render::buffer::Manager::get_buffer() const
{
    return buff;
}

gearoenix::render::buffer::Buffer* gearoenix::render::buffer::Manager::get_buffer()
{
    return buff;
}

gearoenix::render::buffer::SubBuffer* gearoenix::render::buffer::Manager::create_subbuffer(unsigned int size)
{
    if (0 != (coalign & size))
        size = (size & decoalign) + align;
    SubBuffer* subbuff = new SubBuffer(size, buff);
    allocate(subbuff);
    return subbuff;
}
