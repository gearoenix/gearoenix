#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../gx-vk-loader.hpp"
#include "../memory/gx-vk-mem-place.hpp"

#include <memory>
#include <string>

namespace gearoenix::vulkan::memory {
struct Memory;
}

namespace gearoenix::vulkan::buffer {
struct Buffer final {
private:
    const std::uint32_t offset;
    const std::shared_ptr<const Buffer> parent;
    const std::shared_ptr<memory::Memory> allocated_memory;
    const vk::Buffer vulkan_data;
    std::weak_ptr<Buffer> self;

    Buffer(std::uint32_t offset, std::shared_ptr<const Buffer>&& parent, std::shared_ptr<memory::Memory>&& allocated_memory, vk::Buffer vulkan_data);

public:
    Buffer(Buffer&&) = delete;
    Buffer(const Buffer&) = delete;
    Buffer& operator=(Buffer&&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    ~Buffer();

    [[nodiscard]] std::uint32_t get_offset() const { return offset; }
    [[nodiscard]] const std::shared_ptr<const Buffer>& get_parent() const { return parent; }
    [[nodiscard]] const std::shared_ptr<memory::Memory>& get_allocated_memory() const { return allocated_memory; }
    [[nodiscard]] vk::Buffer get_vulkan_data() const { return vulkan_data; }

    [[nodiscard]] static std::shared_ptr<Buffer> construct(const std::string&, std::int64_t size, memory::Place place);
    [[nodiscard]] std::shared_ptr<Buffer> allocate(std::int64_t size);
    void write(const void* data, std::int64_t size);
    [[nodiscard]] vk::DeviceAddress get_device_address() const;
};
}
#endif
