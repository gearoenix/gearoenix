#ifndef GEAROENIX_VULKAN_BUFFER_BUFFER_HPP
#define GEAROENIX_VULKAN_BUFFER_BUFFER_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"
#include "../memory/gx-vk-mem-memory.hpp"
#include "../memory/gx-vk-mem-place.hpp"
#include <memory>
#include <string>

namespace gearoenix::core {
struct Allocator;
}

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::vulkan::memory {
struct Manager;
}

namespace gearoenix::vulkan::buffer {
struct Buffer final {
    GX_GET_CREF_PRT(std::shared_ptr<core::Allocator>, allocator)
    GX_GET_REFC_PRT(std::shared_ptr<const Buffer>, parent)
    GX_GET_CREF_PRT(std::shared_ptr<memory::Memory>, allocated_memory)
    GX_GET_VAL_PRT(VkBuffer, vulkan_data, nullptr)

private:
    std::weak_ptr<Buffer> self;

    Buffer(
        std::shared_ptr<core::Allocator> allocator,
        std::shared_ptr<const Buffer> parent,
        std::shared_ptr<memory::Memory> allocated_memory,
        VkBuffer vulkan_data) noexcept;

public:
    Buffer(Buffer&&) = delete;
    Buffer(const Buffer&) = delete;
    Buffer& operator=(Buffer&&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    [[nodiscard]] static std::shared_ptr<Buffer> construct(
        const std::string&, std::size_t size, memory::Place place, memory::Manager& memory_manager) noexcept;
    ~Buffer() noexcept;
    [[nodiscard]] std::shared_ptr<Buffer> allocate(std::size_t size) noexcept;
    // void push_memory_barrier(command::Buffer& command) const noexcept;
    //[[nodiscard]] static std::uint32_t get_memory_type_bits(device::Logical& device, bool in_gpu) noexcept;
    void write(const void* data, std::size_t size) noexcept;
    [[nodiscard]] VkDeviceAddress get_device_address() const noexcept;
};
}
#endif
#endif
