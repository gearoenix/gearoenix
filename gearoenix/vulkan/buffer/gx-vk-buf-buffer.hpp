#ifndef GEAROENIX_VULKAN_BUFFER_BUFFER_HPP
#define GEAROENIX_VULKAN_BUFFER_BUFFER_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan::command {
class Buffer;
}

namespace gearoenix::vulkan::device {
class Logical;
}

namespace gearoenix::vulkan::memory {
class Manager;
class SubMemory;
}

namespace gearoenix::vulkan::buffer {
class Buffer {
private:
    device::Logical* device;
    memory::SubMemory* smem;
    void* buffer_data;
    VkBuffer vulkan_data;

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
