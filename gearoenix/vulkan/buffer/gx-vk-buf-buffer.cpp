#include "gx-vk-buf-buffer.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-allocator.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../gx-vk-check.hpp"
#include "../memory/gx-vk-mem-manager.hpp"

gearoenix::vulkan::buffer::Buffer::Buffer(
    std::shared_ptr<core::Allocator> alc,
    Buffer* const prt,
    memory::Memory&& mem,
    VkBuffer vulkan_data) noexcept
    : allocator(std::move(alc))
    , parent(prt)
    , allocated_memory(std::move(mem))
    , vulkan_data(vulkan_data)
{
}

gearoenix::vulkan::buffer::Buffer::Buffer(Buffer&&) noexcept = default;

gearoenix::vulkan::buffer::Buffer gearoenix::vulkan::buffer::Buffer::construct(
    const std::size_t size,
    const memory::Place place,
    memory::Manager& memory_manager) noexcept
{
    auto* const logical_device = memory_manager.get_logical_device().get();
    const auto aligned_size = logical_device->get_physical_device()->align_size(size);
    auto allocator = core::Allocator::construct(aligned_size, 0, 0);
    VkBufferCreateInfo info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.size = static_cast<VkDeviceSize>(aligned_size);
    info.usage = place == memory::Place::Cpu ? VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT : VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    VkBuffer vulkan_data = nullptr;
    VkDevice dev = logical_device->get_vulkan_data();
    GX_VK_CHK_L(vkCreateBuffer(dev, &info, nullptr, &vulkan_data))
    VkMemoryRequirements mem_req;
    Loader::vkGetBufferMemoryRequirements(dev, vulkan_data, &mem_req);
    auto allocated_memory = memory_manager.allocate(aligned_size, mem_req.memoryTypeBits, place);
    GX_VK_CHK_L(vkBindBufferMemory(
        dev, vulkan_data, allocated_memory.get_vulkan_data(),
        allocated_memory.get_allocator()->get_root_offset()))
    return Buffer(std::move(allocator), nullptr, std::move(allocated_memory), vulkan_data);
}

gearoenix::vulkan::buffer::Buffer::~Buffer() noexcept
{
    if (nullptr == parent)
        Loader::vkDestroyBuffer(
            allocated_memory.get_manager()->get_logical_device()->get_vulkan_data(), vulkan_data, nullptr);
}

gearoenix::vulkan::buffer::Buffer gearoenix::vulkan::buffer::Buffer::allocate(const std::size_t size) noexcept
{
    const auto aligned_size = allocated_memory.get_manager()->get_logical_device()->get_physical_device()->align_size(size);
    return Buffer(
        allocator->allocate(aligned_size),
        this,
        allocated_memory.allocate(aligned_size),
        vulkan_data);
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
