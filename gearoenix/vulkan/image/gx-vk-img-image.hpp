#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"

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
struct Memory;
struct Manager;
}

namespace gearoenix::vulkan::image {
struct Image final {
    GX_GET_CREF_PRV(std::shared_ptr<memory::Memory>, allocated_memory);
    GX_GET_VAL_PRV(std::uint32_t, image_width, 0);
    GX_GET_VAL_PRV(std::uint32_t, image_height, 0);
    GX_GET_VAL_PRV(std::uint32_t, image_depth, 0);
    GX_GET_VAL_PRV(std::uint32_t, mipmap_level, 0);
    GX_GET_VAL_PRV(std::uint32_t, array_layers, 0);
    GX_GET_VAL_PRV(VkFormat, format, VK_FORMAT_UNDEFINED);
    GX_GET_VAL_PRV(VkImageCreateFlags, flags, 0);
    GX_GET_VAL_PRV(VkImageUsageFlags, usage, 0);
    GX_GET_VAL_PRV(VkImage, vulkan_data, nullptr);

    void terminate();

public:
    Image(Image&&) = delete;
    Image(const Image&) = delete;
    Image& operator=(Image&&) = delete;
    Image& operator=(const Image&) = delete;
    Image(
        std::uint32_t image_width,
        std::uint32_t image_height,
        std::uint32_t image_depth,
        std::uint32_t mipmap_level,
        std::uint32_t array_layers,
        VkFormat format,
        VkImageCreateFlags flags,
        VkImageUsageFlags usage,
        VkImage vulkan_data);
    Image(
        std::uint32_t image_width,
        std::uint32_t image_height,
        std::uint32_t image_depth,
        std::uint32_t mipmap_level,
        std::uint32_t array_layers,
        VkFormat format,
        VkImageCreateFlags flags,
        VkImageUsageFlags usage);
    ~Image();
    //    void transit(command::Buffer& cmd, const VkImageLayout& old_lyt, const VkImageLayout& new_lyt) ;
    //    void transit_for_writing(command::Buffer& cmd) ;
    //    void copy_from_buffer(command::Buffer& cmd, const buffer::Buffer& buf) ;
    //    void transit_for_reading(command::Buffer& cmd) ;
};
}
#endif
