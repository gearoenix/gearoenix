#include "gx-vk-buf-buffer.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/allocator/gx-cr-alc-range.hpp"
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../device/gx-vk-dev-physical.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-marker.hpp"
#include "../memory/gx-vk-mem-manager.hpp"
#include "../memory/gx-vk-mem-memory.hpp"

gearoenix::vulkan::buffer::Buffer::Buffer(const std::uint32_t offset, std::shared_ptr<const Buffer>&& parent, std::shared_ptr<memory::Memory>&& allocated_memory, const vk::Buffer vulkan_data)
    : offset(offset)
    , parent(std::move(parent))
    , allocated_memory(std::move(allocated_memory))
    , vulkan_data(vulkan_data)
{
}

std::shared_ptr<gearoenix::vulkan::buffer::Buffer> gearoenix::vulkan::buffer::Buffer::construct([[maybe_unused]] const std::string& name, const std::int64_t size, const memory::Place place)
{
    const auto& logical_device = device::Logical::get();
    const auto& physical_device = device::Physical::get();
    const auto aligned_size = size;

    vk::BufferUsageFlags usage;
    if (memory::Place::Cpu == place) {
        usage = vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst;
    } else {
        usage = vk::BufferUsageFlagBits::eVertexBuffer
            | vk::BufferUsageFlagBits::eIndexBuffer
            | vk::BufferUsageFlagBits::eUniformBuffer
            | vk::BufferUsageFlagBits::eTransferDst
            | vk::BufferUsageFlagBits::eStorageBuffer;
        if (physical_device.get_rtx_supported()) {
            usage |= vk::BufferUsageFlagBits::eAccelerationStructureBuildInputReadOnlyKHR
                | vk::BufferUsageFlagBits::eAccelerationStructureStorageKHR
                | vk::BufferUsageFlagBits::eShaderDeviceAddress;
        }
    }

    const vk::BufferCreateInfo info({ }, static_cast<vk::DeviceSize>(aligned_size), usage);
    const auto dev = logical_device.get_vulkan_data();
    const auto vulkan_data = dev.createBuffer(info);

    const auto mem_req = dev.getBufferMemoryRequirements(vulkan_data);
    auto allocated_memory = memory::Manager::get().allocate(
        static_cast<std::int64_t>(mem_req.size), static_cast<std::int64_t>(mem_req.alignment), mem_req.memoryTypeBits, place);
    if (nullptr == allocated_memory) {
        dev.destroyBuffer(vulkan_data);
        return nullptr;
    }
    dev.bindBufferMemory(vulkan_data, allocated_memory->get_vulkan_data(), allocated_memory->get_allocator()->get_offset());
    GX_VK_MARK(name, vulkan_data);
    std::shared_ptr<Buffer> result(new Buffer(0, nullptr, std::move(allocated_memory), vulkan_data));
    result->self = result;
    return result;
}

gearoenix::vulkan::buffer::Buffer::~Buffer()
{
    if (!parent && vulkan_data) {
        device::Logical::get().get_vulkan_data().destroyBuffer(vulkan_data);
    }
}

std::shared_ptr<gearoenix::vulkan::buffer::Buffer> gearoenix::vulkan::buffer::Buffer::allocate(const std::int64_t size, const std::int64_t alignment)
{
    auto alc_mem = allocated_memory->allocate(size, alignment);
    if (nullptr == alc_mem) {
        return nullptr;
    }

    const auto mem_off = alc_mem->get_allocator()->get_offset() - allocated_memory->get_allocator()->get_offset();
    GX_ASSERT_D(mem_off % alignment == 0); // the final memory must be aligned, if it wasn't, maybe you need to choose a better offsetting for your root buffer

    std::shared_ptr<Buffer> result(new Buffer(offset + static_cast<std::uint32_t>(mem_off), self.lock(), std::move(alc_mem), vulkan_data));
    result->self = result;
    return result;
}

std::shared_ptr<gearoenix::vulkan::buffer::Buffer> gearoenix::vulkan::buffer::Buffer::allocate(const std::int64_t size)
{
    return allocate(size, static_cast<std::int64_t>(device::Physical::get().get_properties().limits.optimalBufferCopyOffsetAlignment));
}

void gearoenix::vulkan::buffer::Buffer::write(const void* const data, const std::int64_t size)
{
    GX_ASSERT_D(nullptr != allocated_memory->get_data());
    std::memcpy(allocated_memory->get_data(), data, size);
}

vk::DeviceAddress gearoenix::vulkan::buffer::Buffer::get_device_address() const
{
    const vk::BufferDeviceAddressInfo addr_info { vulkan_data };
    return device::Logical::get().get_device().getBufferAddress(addr_info) + static_cast<vk::DeviceAddress>(offset);
}

#endif
