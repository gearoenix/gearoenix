#ifndef GEAROENIX_VULKAN_MEMORY_MANAGER_HPP
#define GEAROENIX_VULKAN_MEMORY_MANAGER_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/gx-cr-static.hpp"
#include "../gx-vk-loader.hpp"
#include "gx-vk-mem-usage.hpp"

namespace gearoenix::vulkan::device {
class Logical;
}

namespace gearoenix::vulkan::buffer {
class Buffer;
}

namespace gearoenix::vulkan::memory {
class Memory;
class Manager final {
public:
    GX_GET_REFC_PRV(std::shared_ptr<device::Logical>, logical_device)
    GX_GET_CREF_PRV(std::weak_ptr<Manager>, self)
private:
    explicit Manager(std::shared_ptr<device::Logical> logical_device) noexcept;

public:
    [[nodiscard]] static std::shared_ptr<Manager> construct(std::shared_ptr<device::Logical> logical_device) noexcept;
    ~Manager() noexcept;

    [[nodiscard]] std::tuple<VkImage, std::shared_ptr<Memory>> create(const VkImageCreateInfo& info) noexcept;
    void destroy(VkImage image, std::shared_ptr<Memory>& mem) noexcept;

    [[nodiscard]] std::tuple<VkBuffer, std::shared_ptr<Memory>, void*> create(const VkBufferCreateInfo& info, Usage usage) noexcept;
    void destroy(VkBuffer, std::shared_ptr<Memory>& mem) noexcept;

    //    [[nodiscard]] static VkMemoryUsage convert(Usage usage) noexcept;
};
}
#endif
#endif
