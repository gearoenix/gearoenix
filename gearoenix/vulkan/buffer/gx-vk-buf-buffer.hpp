#ifndef GEAROENIX_VULKAN_BUFFER_BUFFER_HPP
#define GEAROENIX_VULKAN_BUFFER_BUFFER_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/gx-cr-static.hpp"
#include "../gx-vk-loader.hpp"
#include "../memory/gx-vk-mem-place.hpp"

namespace gearoenix::core {
class Allocator;
}

namespace gearoenix::vulkan::command {
class Buffer;
}

namespace gearoenix::vulkan::engine {
class Engine;
}

namespace gearoenix::vulkan::memory {
class Manager;
class Memory;
}

namespace gearoenix::vulkan::buffer {
class Buffer final {
    GX_GET_REFC_PRT(std::shared_ptr<core::Allocator>, allocator)
    GX_GET_REFC_PRT(std::shared_ptr<Buffer>, parent)
    GX_GET_REFC_PRT(std::shared_ptr<memory::Memory>, allocated_memory)
    GX_GET_VAL_PRT(VkBuffer, vulkan_data, nullptr)
protected:
    std::weak_ptr<Buffer> self;

    Buffer(
        std::shared_ptr<core::Allocator> allocator,
        std::shared_ptr<Buffer> parent,
        std::shared_ptr<memory::Memory> allocated_memory,
        VkBuffer vulkan_data) noexcept;

public:
    [[nodiscard]] static std::shared_ptr<Buffer> construct(
        std::size_t size, memory::Place place, memory::Manager& memory_manager) noexcept;
    ~Buffer() noexcept;
    [[nodiscard]] std::shared_ptr<Buffer> allocate(std::size_t size) noexcept;
    //    void copy(command::Buffer& command, const Buffer& src) noexcept;
    //    void push_memory_barrier(command::Buffer& command) const noexcept;
    //    [[nodiscard]] static std::uint32_t get_memory_type_bits(device::Logical& device, bool in_gpu) noexcept;
    //    void write(const void* data, unsigned int data_size, unsigned int offset) noexcept;
};
}
#endif
#endif
