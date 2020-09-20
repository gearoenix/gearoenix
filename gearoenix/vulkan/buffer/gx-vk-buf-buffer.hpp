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
        memory::Manager& mem_mgr,
        std::uint32_t size,
        std::uint32_t usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) noexcept;
    ~Buffer() noexcept;
    void copy(command::Buffer& command, const Buffer& src) noexcept;
    void push_memory_barrier(command::Buffer& command) const noexcept;
    [[nodiscard]] static std::uint32_t get_memory_type_bits(device::Logical& device, bool in_gpu) noexcept;
    void write(const void* data, unsigned int data_size, unsigned int offset) noexcept;
};
}
#endif
#endif
