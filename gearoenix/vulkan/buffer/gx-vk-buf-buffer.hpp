#ifndef GEAROENIX_VULKAN_BUFFER_BUFFER_HPP
#define GEAROENIX_VULKAN_BUFFER_BUFFER_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/gx-cr-static.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan::command {
class Buffer;
}

namespace gearoenix::vulkan::device {
class Logical;
}

namespace gearoenix::vulkan::memory {
class Manager;
class Memory;
}

namespace gearoenix::vulkan::buffer {
class Buffer final {
    GX_GET_CREF_PRV(std::shared_ptr<memory::Memory>, allocated_memory)
    GX_GET_VAL_PRV(VkBuffer, vulkan_data, nullptr)
    GX_GET_VAL_PRV(std::uint32_t, offset, 0)

public:
    Buffer(
        memory::Manager* mem_mgr, unsigned int size,
        uint32_t usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    ~Buffer();
    const VkBuffer& get_vulkan_data() const;
    void copy(command::Buffer* command, Buffer* src);
    void push_memory_barrier(command::Buffer* command) const;
    const device::Logical* get_logical_device() const;
    device::Logical* get_logical_device();
    static uint32_t get_memory_type_bits(device::Logical* device, bool in_gpu);
    void write(const void* data, unsigned int data_size, unsigned int offset);
};
}
#endif
#endif
