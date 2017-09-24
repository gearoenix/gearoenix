#include "vk-buf-buffer.hpp"
#include "../../core/cr-static.hpp"
#include "../command/vk-cmd-buffer.hpp"
#include "../command/vk-cmd-pool.hpp"
#include "../device/vk-dev-logical.hpp"
#include "../device/vk-dev-physical.hpp"
#include "../memory/vk-mem-manager.hpp"
#include "../memory/vk-mem-memory.hpp"
#include "../memory/vk-mem-sub-memory.hpp"
#include "../vk-check.hpp"
#include "../vk-instance.hpp"

gearoenix::render::buffer::Buffer::Buffer(memory::Manager* mem_mgr, unsigned int size, uint32_t usage)
{
    memory::Memory* main_mem = mem_mgr->get_memory();
    device = main_mem->get_logical_device();
    bool is_in_gpu = main_mem->get_is_in_gpu();
    const device::Physical* p = device->get_physical_device();
    const Instance* ins = p->get_instance();
    const Linker* l = ins->get_linker();
    const VkDevice vkdev = device->get_vulkan_data();
    VkBufferCreateInfo vertex_buffer_info;
    setz(vertex_buffer_info);
    vertex_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vertex_buffer_info.size = size;
    if (is_in_gpu) {
        vertex_buffer_info.usage = usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    } else {
        vertex_buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    }
    VKC(l->vkCreateBuffer(vkdev, &vertex_buffer_info, nullptr, &vulkan_data));
    VkMemoryRequirements memreqs;
    l->vkGetBufferMemoryRequirements(vkdev, vulkan_data, &memreqs);
    if (size != memreqs.size)
        LOGE("Strange! size: " << size << " and memreqs.size: " << memreqs.size);
    smem = mem_mgr->create_submemory(memreqs.size);
    const VkDeviceMemory vkmem = main_mem->get_vulkan_data();
    VkDeviceSize offset = smem->get_offset();
    if (is_in_gpu) {
        buffer_data = nullptr;
    } else {
        VKC(l->vkMapMemory(vkdev, vkmem, offset, memreqs.size, 0, &buffer_data));
    }
    VKC(l->vkBindBufferMemory(vkdev, vulkan_data, vkmem, offset));
}

gearoenix::render::buffer::Buffer::~Buffer()
{
    const device::Physical* p = device->get_physical_device();
    const Instance* ins = p->get_instance();
    const Linker* l = ins->get_linker();
    const VkDevice vkdev = device->get_vulkan_data();
    l->vkDestroyBuffer(vkdev, vulkan_data, nullptr);
    delete smem;
}

void gearoenix::render::buffer::Buffer::copy(command::Buffer* copy_cmd, Buffer* src)
{
    VkBufferCopy copy_region;
    setz(copy_region);
    copy_region.dstOffset = smem->get_offset();
    copy_region.size = smem->get_size();
    copy_region.srcOffset = src->smem->get_offset();
    copy_cmd->copy_buffer(src->vulkan_data, vulkan_data, copy_region);
}

const VkBuffer& gearoenix::render::buffer::Buffer::get_vulkan_data() const
{
    return vulkan_data;
}

void gearoenix::render::buffer::Buffer::push_memory_barrier(command::Buffer*) const
{
    LOGF("Unimplemented");
}

const gearoenix::render::device::Logical* gearoenix::render::buffer::Buffer::get_logical_device() const
{
    return device;
}

gearoenix::render::device::Logical* gearoenix::render::buffer::Buffer::get_logical_device()
{
    return device;
}

uint32_t gearoenix::render::buffer::Buffer::get_memory_type_bits(device::Logical* device, bool in_gpu)
{
    const device::Physical* p = device->get_physical_device();
    const Instance* ins = p->get_instance();
    const Linker* l = ins->get_linker();
    const VkDevice vkdev = device->get_vulkan_data();
    VkBufferCreateInfo vertex_buffer_info;
    setz(vertex_buffer_info);
    vertex_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vertex_buffer_info.size = 1024 * 1024;
    if (in_gpu) {
        vertex_buffer_info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    } else {
        vertex_buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    }
    VkBuffer vulkan_data;
    VKC(l->vkCreateBuffer(vkdev, &vertex_buffer_info, nullptr, &vulkan_data));
    VkMemoryRequirements memreqs;
    l->vkGetBufferMemoryRequirements(vkdev, vulkan_data, &memreqs);
    l->vkDestroyBuffer(vkdev, vulkan_data, nullptr);
    return memreqs.memoryTypeBits;
}
