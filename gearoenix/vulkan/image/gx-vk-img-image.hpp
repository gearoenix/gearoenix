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
struct Image final {
private:
    GX_GET_CPTR_PRV(device::Logical, logical_device)
    GX_GET_CREF_PRV(std::optional<memory::Memory>, allocated_memory)
    GX_GET_VAL_PRV(std::uint32_t, image_width, 0)
    GX_GET_VAL_PRV(std::uint32_t, image_height, 0)
    GX_GET_VAL_PRV(std::uint32_t, image_depth, 0)
    GX_GET_VAL_PRV(std::uint32_t, mipmap_level, 0)
    GX_GET_VAL_PRV(std::uint32_t, array_layers, 0)
    GX_GET_VAL_PRV(VkFormat, format, VK_FORMAT_UNDEFINED)
    GX_GET_VAL_PRV(VkImageCreateFlags, flags, 0)
    GX_GET_VAL_PRV(VkImageUsageFlags, usage, 0)
    GX_GET_VAL_PRV(VkImage, vulkan_data, nullptr)

public:
    Image(const Image&) = delete;
    Image(Image&&) noexcept;
    Image(
        const device::Logical* logical_device,
        std::uint32_t image_width,
        std::uint32_t image_height,
        std::uint32_t image_depth,
        std::uint32_t mipmap_level,
        std::uint32_t array_layers,
        VkFormat format,
        VkImageCreateFlags flags,
        VkImageUsageFlags usage,
        VkImage vulkan_data) noexcept;
    Image(
        std::uint32_t image_width,
        std::uint32_t image_height,
        std::uint32_t image_depth,
        std::uint32_t mipmap_level,
        std::uint32_t array_layers,
        VkFormat format,
        VkImageCreateFlags flags,
        VkImageUsageFlags usage,
        memory::Manager& memory_manager) noexcept;
    ~Image() noexcept;
    Image& operator=(const Image&) = delete;
    Image& operator=(Image&&) noexcept;
    //    void transit(command::Buffer& cmd, const VkImageLayout& old_lyt, const VkImageLayout& new_lyt) noexcept;
    //    void transit_for_writing(command::Buffer& cmd) noexcept;
    //    void copy_from_buffer(command::Buffer& cmd, const buffer::Buffer& buf) noexcept;
    //    void transit_for_reading(command::Buffer& cmd) noexcept;
};
}
#endif
#endif
