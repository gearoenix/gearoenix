#ifndef GEAROENIX_VULKAN_IMAGE_IMAGE_HPP
#define GEAROENIX_VULKAN_IMAGE_IMAGE_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/gx-cr-static.hpp"
#include "../gx-vk-loader.hpp"

#include <memory>

namespace gearoenix::vulkan::buffer {
class Buffer;
}

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

namespace gearoenix::vulkan::image {
class Image {
private:
    GX_GET_REFC_PRV(std::shared_ptr<device::Logical>, logical_device)
    GX_GET_CREF_PRV(std::shared_ptr<memory::Memory>, allocated_memory)
    GX_GET_VAL_PRV(VkImage, vulkan_data, nullptr)
    GX_GET_CVAL_PRV(std::uint32_t, image_width)
    GX_GET_CVAL_PRV(std::uint32_t, image_height)
    GX_GET_CVAL_PRV(VkFormat, format)
    GX_GET_CVAL_PRV(VkImageUsageFlags, usage)

public:
    Image(
        std::shared_ptr<device::Logical> logical_device,
        VkImage vulkan_data,
        std::uint32_t image_width,
        std::uint32_t image_height,
        VkImageUsageFlags usage,
        VkFormat format,
        std::shared_ptr<memory::Memory> allocated_memory = nullptr) noexcept;

    Image(
        std::uint32_t image_width,
        std::uint32_t image_height,
        std::uint32_t image_depth,
        std::uint32_t mipmap_level,
        std::uint32_t array_layers,
        VkImageUsageFlags usage,
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
