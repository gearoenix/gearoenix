#ifndef GEAROENIX_VULKAN_MEMORY_MANAGER_HPP
#define GEAROENIX_VULKAN_MEMORY_MANAGER_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/gx-cr-static.hpp"
#include "../gx-vk-loader.hpp"

#include "gx-vk-mem-vma.hpp"

namespace gearoenix::vulkan::device {
class Logical;
}

namespace gearoenix::vulkan::buffer {
class Buffer;
}

namespace gearoenix::vulkan::memory {
class Memory;
class Manager final : std::enable_shared_from_this<Manager> {
    GX_GET_REFC_PRV(std::shared_ptr<device::Logical>, logical_device)
    GX_GET_VAL_PRV(VmaAllocator, allocator, nullptr)

    explicit Manager(std::shared_ptr<device::Logical> logical_device) noexcept;

public:
    ~Manager() noexcept;
    [[nodiscard]] std::tuple<VkImage, std::shared_ptr<Memory>> create_image(const VkImageCreateInfo& info) noexcept;
    void destroy_image(VkImage image, std::shared_ptr<Memory>& mem) noexcept;
    // [[nodiscard]] std::shared_ptr<buffer::Buffer>
};
}
#endif
#endif
