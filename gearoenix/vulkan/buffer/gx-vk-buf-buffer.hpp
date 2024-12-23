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

namespace gearoenix::core::allocator {
struct Range;
}

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::vulkan::memory {
struct Manager;
}

namespace gearoenix::vulkan::buffer {
struct Buffer final {
    GX_GET_CREF_PRT(std::shared_ptr<core::allocator::Range>, allocator);
    GX_GET_REFC_PRT(std::shared_ptr<const Buffer>, parent);
    GX_GET_CREF_PRT(std::shared_ptr<memory::Memory>, allocated_memory);
    GX_GET_VAL_PRT(VkBuffer, vulkan_data, nullptr);

private:
    std::weak_ptr<Buffer> self;

    Buffer(
        std::shared_ptr<core::allocator::Range> allocator,
        std::shared_ptr<const Buffer> parent,
        std::shared_ptr<memory::Memory> allocated_memory,
        VkBuffer vulkan_data);

public:
    Buffer(Buffer&&) = delete;
    Buffer(const Buffer&) = delete;
    Buffer& operator=(Buffer&&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    [[nodiscard]] static std::shared_ptr<Buffer> construct(
        const std::string&, std::uint64_t size, memory::Place place, memory::Manager& memory_manager);
    ~Buffer();
    [[nodiscard]] std::shared_ptr<Buffer> allocate(std::uint64_t size);
    // void push_memory_barrier(command::Buffer& command) const ;
    //[[nodiscard]] static std::uint32_t get_memory_type_bits(device::Logical& device, bool in_gpu) ;
    void write(const void* data, std::uint64_t size);
    [[nodiscard]] VkDeviceAddress get_device_address() const;
};
}
#endif
#endif
