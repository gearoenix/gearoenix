#include "gx-vk-buf-buffer.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-allocator.hpp"
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-check.hpp"
#include "../gx-vk-marker.hpp"

gearoenix::vulkan::buffer::Buffer::Buffer(
    std::shared_ptr<core::Allocator> allocator,
    std::shared_ptr<const Buffer> parent,
    std::shared_ptr<memory::Memory> allocated_memory,
    VkBuffer vulkan_data) noexcept
    : allocator(std::move(allocator))
    , parent(std::move(parent))
    , allocated_memory(std::move(allocated_memory))
    , vulkan_data(vulkan_data)
{
}

std::shared_ptr<gearoenix::vulkan::buffer::Buffer> gearoenix::vulkan::buffer::Buffer::construct(
    const std::string& name,
    const std::size_t size,
    const memory::Place place,
    memory::Manager& memory_manager) noexcept
{
    const auto& logical_device = memory_manager.get_e().get_logical_device();
    const auto& physical_device = logical_device.get_physical_device();
    const auto aligned_size = physical_device.align_size(size);
    auto allocator = core::Allocator::construct(aligned_size);
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
            info.usage |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR;
            info.usage |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR;
            info.usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        }
    }
    VkBuffer vulkan_data;
    VkDevice dev = logical_device.get_vulkan_data();
    GX_VK_CHK(vkCreateBuffer(dev, &info, nullptr, &vulkan_data))
    VkMemoryRequirements mem_req;
    vkGetBufferMemoryRequirements(dev, vulkan_data, &mem_req);
    auto allocated_memory = memory_manager.allocate(aligned_size, mem_req.memoryTypeBits, place);
    if (nullptr == allocated_memory) {
        vkDestroyBuffer(logical_device.get_vulkan_data(), vulkan_data, nullptr);
        return nullptr;
    }
    GX_VK_CHK(vkBindBufferMemory(
        dev, vulkan_data, allocated_memory->get_vulkan_data(), allocated_memory->get_allocator()->get_offset()))
    GX_VK_MARK(name, vulkan_data, logical_device)
    std::shared_ptr<Buffer> result(new Buffer(std::move(allocator), nullptr, std::move(allocated_memory), vulkan_data));
    result->self = result;
    return result;
}

gearoenix::vulkan::buffer::Buffer::~Buffer() noexcept
{
    if (nullptr == parent && nullptr != vulkan_data) {
        vkDestroyBuffer(
            allocated_memory->get_e().get_logical_device().get_vulkan_data(), vulkan_data, nullptr);
        vulkan_data = nullptr;
    }
}

std::shared_ptr<gearoenix::vulkan::buffer::Buffer> gearoenix::vulkan::buffer::Buffer::allocate(const std::size_t size) noexcept
{
    const auto aligned_size = allocated_memory->get_e().get_logical_device().get_physical_device().align_size(size);
    auto alc = allocator->allocate(aligned_size);
    if (nullptr == alc)
        return nullptr;
    auto alc_mem = allocated_memory->allocate(aligned_size);
    if (nullptr == alc_mem)
        return nullptr;
    std::shared_ptr<Buffer> result(new Buffer(std::move(alc), self.lock(), std::move(alc_mem), vulkan_data));
    result->self = result;
    return result;
}

void gearoenix::vulkan::buffer::Buffer::write(const void* data, const std::size_t size) noexcept
{
    GX_CHECK_NOT_EQUAL_D(nullptr, allocated_memory->get_data())
    std::memcpy(allocated_memory->get_data(), data, size);
}

VkDeviceAddress gearoenix::vulkan::buffer::Buffer::get_device_address() const noexcept
{
    VkBufferDeviceAddressInfo info;
    GX_SET_ZERO(info)
    info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    info.buffer = vulkan_data;
    return vkGetBufferDeviceAddress(
               allocated_memory->get_e().get_logical_device().get_vulkan_data(),
               &info)
        + static_cast<VkDeviceAddress>(allocator->get_offset());
}

#endif
