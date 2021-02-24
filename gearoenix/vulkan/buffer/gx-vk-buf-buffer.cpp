#include "gx-vk-buf-buffer.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../gx-vk-check.hpp"
#include "../gx-vk-marker.hpp"
#include "../memory/gx-vk-mem-manager.hpp"

gearoenix::vulkan::buffer::Buffer::Buffer(
    core::Allocator&& allocator,
    Buffer* const parent,
    memory::Memory&& allocated_memory,
    VkBuffer vulkan_data) noexcept
    : allocator(std::move(allocator))
    , parent(parent)
    , allocated_memory(std::move(allocated_memory))
    , vulkan_data(vulkan_data)
{
}

std::optional<gearoenix::vulkan::buffer::Buffer> gearoenix::vulkan::buffer::Buffer::construct(
    const std::string& name,
    const std::size_t size,
    const memory::Place place,
    memory::Manager& memory_manager) noexcept
{
    const auto& logical_device = memory_manager.get_logical_device();
    const auto& physical_device = logical_device.get_physical_device();
    const auto aligned_size = physical_device.align_size(size);
    auto allocator = core::Allocator(aligned_size);
    VkBufferCreateInfo info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.size = static_cast<VkDeviceSize>(aligned_size);
    if (memory::Place::Cpu == place) {
        info.usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    } else {
        info.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        info.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        info.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        info.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        if (physical_device.get_rtx_supported()) {
            info.usage |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;
            info.usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
            info.usage |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR;
        }
    }
    VkBuffer vulkan_data;
    VkDevice dev = logical_device.get_vulkan_data();
    GX_VK_CHK(vkCreateBuffer(dev, &info, nullptr, &vulkan_data))
    VkMemoryRequirements mem_req;
    vkGetBufferMemoryRequirements(dev, vulkan_data, &mem_req);
    auto _allocated_memory = memory_manager.allocate(aligned_size, mem_req.memoryTypeBits, place);
    if (!_allocated_memory.has_value()) {
        vkDestroyBuffer(logical_device.get_vulkan_data(), vulkan_data, nullptr);
        return std::nullopt;
    }
    auto allocated_memory = std::move(*_allocated_memory);
    GX_VK_CHK(vkBindBufferMemory(
        dev, vulkan_data, allocated_memory.get_vulkan_data(), allocated_memory.get_allocator().get_offset()))
    GX_VK_MARK(name, vulkan_data, logical_device)
    return Buffer(std::move(allocator), nullptr, std::move(allocated_memory), vulkan_data);
}

gearoenix::vulkan::buffer::Buffer::~Buffer() noexcept
{
    if (nullptr == parent && nullptr != vulkan_data) {
        vkDestroyBuffer(
            allocated_memory.get_manager()->get_logical_device().get_vulkan_data(), vulkan_data, nullptr);
        vulkan_data = nullptr;
    }
}

std::optional<gearoenix::vulkan::buffer::Buffer> gearoenix::vulkan::buffer::Buffer::allocate(const std::size_t size) noexcept
{
    const auto aligned_size = allocated_memory.get_manager()->get_logical_device().get_physical_device().align_size(size);
    auto alc = allocator.allocate(aligned_size);
    if (!alc.has_value())
        return std::nullopt;
    auto alc_mem = allocated_memory.allocate(aligned_size);
    if (!alc_mem.has_value())
        return std::nullopt;
    return Buffer(std::move(*alc), this, std::move(*alc_mem), vulkan_data);
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

void gearoenix::vulkan::buffer::Buffer::write(const void* data, const std::size_t size) noexcept
{
    GX_CHECK_NOT_EQUAL_D(nullptr, allocated_memory.get_data())
    std::memcpy(allocated_memory.get_data(), data, size);
}

VkDeviceAddress gearoenix::vulkan::buffer::Buffer::get_device_address() const noexcept
{
    VkBufferDeviceAddressInfo info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    info.buffer = vulkan_data;
    return vkGetBufferDeviceAddress(
               allocated_memory.get_manager()->get_logical_device().get_vulkan_data(),
               &info)
        + static_cast<VkDeviceAddress>(allocator.get_offset());
}

#endif
