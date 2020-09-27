#ifndef GEAROENIX_VULKAN_BUFFER_BUFFER_HPP
#define GEAROENIX_VULKAN_BUFFER_BUFFER_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/gx-cr-static.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan::command {
class Buffer;
}

namespace gearoenix::vulkan::engine {
class Engine;
}

namespace gearoenix::vulkan::memory {
class Memory;
}

namespace gearoenix::vulkan::buffer {
class Buffer {
    GX_GET_CREF_PRT(std::shared_ptr<memory::Memory>, allocated_memory)
    GX_GET_VAL_PRT(VkBuffer, vulkan_data, nullptr)
    GX_GET_CVAL_PRT(std::uint32_t, offset)
protected:
    void* const data;
    Buffer(
        VkBuffer vulkan_data,
        std::shared_ptr<memory::Memory> allocated_memory,
        void* data,
        std::uint32_t offset) noexcept;

public:
    virtual ~Buffer() noexcept;
    //    void copy(command::Buffer& command, const Buffer& src) noexcept;
    //    void push_memory_barrier(command::Buffer& command) const noexcept;
    //    [[nodiscard]] static std::uint32_t get_memory_type_bits(device::Logical& device, bool in_gpu) noexcept;
    //    void write(const void* data, unsigned int data_size, unsigned int offset) noexcept;
};
}
#endif
#endif
