#include "gx-vk-buf-buffer.hpp"
#ifdef GX_USE_VULKAN
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../gx-vk-check.hpp"
#include "../memory/gx-vk-mem-manager.hpp"
#include "../memory/gx-vk-mem-memory.hpp"

gearoenix::vulkan::buffer::Buffer::Buffer(
    VkBuffer vd,
    std::shared_ptr<memory::Memory> mem,
    void* const d,
    const std::uint32_t off) noexcept
    : allocated_memory(std::move(mem))
    , vulkan_data(vd)
    , offset(off)
    , data(d)
{
}

gearoenix::vulkan::buffer::Buffer::~Buffer() noexcept
{
    allocated_memory->get_manager()->destroy(vulkan_data, allocated_memory);
}

//void gearoenix::render::buffer::Buffer::copy(command::Buffer* copy_cmd, Buffer* src)
//{
//    VkBufferCopy copy_region;
//    setz(copy_region);
//    copy_region.dstOffset = smem->get_offset();
//    copy_region.size = smem->get_size();
//    copy_region.srcOffset = src->smem->get_offset();
//    copy_cmd->copy_buffer(src->vulkan_data, vulkan_data, copy_region);
//}
//
//void gearoenix::render::buffer::Buffer::push_memory_barrier(command::Buffer*) const
//{
//    LOGF("Unimplemented");
//}
//
//uint32_t gearoenix::render::buffer::Buffer::get_memory_type_bits(device::Logical* device, bool in_gpu)
//{
//    const device::Physical* p = device->get_physical_device();
//    const Instance* ins = p->get_instance();
//    const Linker* l = ins->get_linker();
//    const VkDevice vkdev = device->get_vulkan_data();
//    VkBufferCreateInfo vertex_buffer_info;
//    setz(vertex_buffer_info);
//    vertex_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//    vertex_buffer_info.size = 1024 * 1024;
//    if (in_gpu) {
//        vertex_buffer_info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
//    } else {
//        vertex_buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
//    }
//    VkBuffer vulkan_data;
//    VKC(l->vkCreateBuffer(vkdev, &vertex_buffer_info, nullptr, &vulkan_data));
//    VkMemoryRequirements memreqs;
//    l->vkGetBufferMemoryRequirements(vkdev, vulkan_data, &memreqs);
//    l->vkDestroyBuffer(vkdev, vulkan_data, nullptr);
//    return memreqs.memoryTypeBits;
//}
//
//void gearoenix::render::buffer::Buffer::write(const void* data, unsigned int data_size, unsigned int offset)
//{
//#ifdef DEBUG_MODE
//    if (nullptr == buffer_data) {
//        LOGF("Unexpected");
//    }
//#endif
//    std::memcpy((void*)((size_t)buffer_data + (size_t)offset), data, data_size);
//}
#endif
