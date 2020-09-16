#ifndef GEAROENIX_VULKAN_IMAGE_IMAGE_HPP
#define GEAROENIX_VULKAN_IMAGE_IMAGE_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/gx-cr-static.hpp"
#include "../gx-vk-loader.hpp"
#include <memory>

namespace gearoenix::vulkan::buffer {
class SubBuffer;
}

namespace gearoenix::vulkan::command {
class Buffer;
}

namespace gearoenix::vulkan::device {
class Logical;
}

namespace gearoenix::vulkan::memory {
class SubMemory;
class Manager;
class Memory;
}

namespace gearoenix::vulkan::image {
class Image {
private:
    GX_GET_REFC_PRV(std::shared_ptr<device::Logical>, logical_device)
    GX_GET_REFC_PRV(std::shared_ptr<memory::Memory>, mem)
    GX_GET_CREF_PRV(std::shared_ptr<memory::SubMemory>, sub_memory)
    GX_GET_VAL_PRV(VkImage, vulkan_data, nullptr)
    GX_GET_VAL_PRV(std::uint32_t, image_width, 0)
    GX_GET_VAL_PRV(std::uint32_t, image_height, 0)
    GX_GET_VAL_PRV(VkFormat, format, VK_FORMAT_UNDEFINED)

public:
    Image(
        std::shared_ptr<device::Logical> logical_device,
        VkImage vulkan_data,
        std::shared_ptr<memory::Memory> mem = nullptr) noexcept;
    Image(
        std::shared_ptr<device::Logical> logical_device,
        const VkImageCreateInfo& info,
        std::shared_ptr<memory::Manager> mem_manager = nullptr) noexcept;
    ~Image() noexcept;
    void transit(command::Buffer& cmd, const VkImageLayout& old_lyt, const VkImageLayout& new_lyt) noexcept;
    void transit_for_writing(command::Buffer& cmd) noexcept;
    void copy_from_buffer(command::Buffer& cmd, buffer::SubBuffer& buf) noexcept;
    void transit_for_reading(command::Buffer& cmd) noexcept;
};
}
#endif
#endif
