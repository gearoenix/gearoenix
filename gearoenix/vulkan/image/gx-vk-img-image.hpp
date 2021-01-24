#ifndef GEAROENIX_VULKAN_IMAGE_IMAGE_HPP
#define GEAROENIX_VULKAN_IMAGE_IMAGE_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"
#include "../memory/gx-vk-mem-memory.hpp"
#include <memory>

namespace gearoenix::vulkan::buffer {
struct Buffer;
}

namespace gearoenix::vulkan::command {
struct Buffer;
}

namespace gearoenix::vulkan::device {
struct Logical;
}

namespace gearoenix::vulkan::memory {
struct Manager;
}

namespace gearoenix::vulkan::image {
struct Image {
private:
    GX_GET_CRRF_PRV(device::Logical, logical_device)
    GX_GET_CREF_PRV(memory::Memory, allocated_memory)
    GX_GET_VAL_PRV(VkImage, vulkan_data, nullptr)
    GX_GET_CVAL_PRV(std::uint32_t, image_width)
    GX_GET_CVAL_PRV(std::uint32_t, image_height)
    GX_GET_CVAL_PRV(VkFormat, format)
    GX_GET_CVAL_PRV(VkImageUsageFlags, usage)

public:
    Image(
        const device::Logical& logical_device,
        VkImage vulkan_data,
        std::uint32_t image_width,
        std::uint32_t image_height,
        VkImageUsageFlags usage,
        VkFormat format) noexcept;

    Image(
        std::uint32_t image_width,
        std::uint32_t image_height,
        std::uint32_t image_depth,
        std::uint32_t mipmap_level,
        std::uint32_t array_layers,
        VkImageUsageFlags usage,
        VkImageCreateFlags flags,
        VkFormat format,
        memory::Manager& memory_manager) noexcept;
    ~Image() noexcept;
    void transit(command::Buffer& cmd, const VkImageLayout& old_lyt, const VkImageLayout& new_lyt) noexcept;
    void transit_for_writing(command::Buffer& cmd) noexcept;
    void copy_from_buffer(command::Buffer& cmd, const buffer::Buffer& buf) noexcept;
    void transit_for_reading(command::Buffer& cmd) noexcept;
};
}
#endif
#endif
