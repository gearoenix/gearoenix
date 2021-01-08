#ifndef GEAROENIX_VULKAN_BUFFER_BUFFER_HPP
#define GEAROENIX_VULKAN_BUFFER_BUFFER_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"
#include "../memory/gx-vk-mem-memory.hpp"
#include "../memory/gx-vk-mem-place.hpp"

namespace gearoenix::core {
struct Allocator;
}

namespace gearoenix::vulkan::command {
struct Buffer;
}

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::vulkan::memory {
struct Manager;
}

namespace gearoenix::vulkan::buffer {
struct Buffer final {
    GX_GET_REFC_PRT(std::shared_ptr<core::Allocator>, allocator)
    GX_GET_PTR_PRT(Buffer, parent)
    GX_GET_REF_PRT(memory::Memory, allocated_memory)
    GX_GET_VAL_PRT(VkBuffer, vulkan_data, nullptr)

protected:
    Buffer(
        std::shared_ptr<core::Allocator> allocator,
        Buffer* parent,
        memory::Memory&& allocated_memory,
        VkBuffer vulkan_data) noexcept;

public:
    Buffer(const Buffer&) noexcept = delete;
    Buffer(Buffer&&) noexcept;
    [[nodiscard]] static Buffer construct(
        std::size_t size, memory::Place place, memory::Manager& memory_manager) noexcept;
    ~Buffer() noexcept;
    [[nodiscard]] Buffer allocate(std::size_t size) noexcept;
    //void copy(command::Buffer& command, const Buffer& src) noexcept;
    //void push_memory_barrier(command::Buffer& command) const noexcept;
    //[[nodiscard]] static std::uint32_t get_memory_type_bits(device::Logical& device, bool in_gpu) noexcept;
    //void write(const void* data, unsigned int data_size, unsigned int offset) noexcept;
};
}
#endif
#endif
