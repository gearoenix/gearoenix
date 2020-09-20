#ifndef GEAROENIX_VULKAN_DEVICE_PHYSICAL_HPP
#define GEAROENIX_VULKAN_DEVICE_PHYSICAL_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/gx-cr-static.hpp"
#include "../gx-vk-loader.hpp"
#include <memory>
#include <vector>

namespace gearoenix::vulkan {
class Instance;
class Surface;
}

namespace gearoenix::vulkan::device {
class Physical final {
    GX_GET_REFC_PRV(std::shared_ptr<Surface>, surface)
    GX_GET_VAL_PRV(VkPhysicalDevice, vulkan_data, nullptr)
    GX_GET_CREF_PRV(VkPhysicalDeviceProperties, properties)
    GX_GET_CREF_PRV(VkPhysicalDeviceFeatures, features)
    GX_GET_CREF_PRV(VkPhysicalDeviceMemoryProperties, memory_properties)
    GX_GET_CREF_PRV(VkSurfaceCapabilitiesKHR, surface_capabilities)
    GX_GET_CREF_PRV(std::vector<VkQueueFamilyProperties>, queue_family_properties)
    GX_GET_CREF_PRV(std::vector<std::string>, supported_extensions)
    GX_GET_CREF_PRV(std::vector<VkSurfaceFormatKHR>, surface_formats)
    GX_GET_VAL_PRV(VkFormat, supported_depth_format, VK_FORMAT_UNDEFINED)
    GX_GET_VAL_PRV(std::uint32_t, max_memory_alignment, std::numeric_limits<std::uint32_t>::max())
    GX_GET_VAL_PRV(std::uint32_t, graphics_queue_node_index, std::numeric_limits<std::uint32_t>::max())
    GX_GET_VAL_PRV(std::uint32_t, transfer_queue_node_index, std::numeric_limits<std::uint32_t>::max())
    GX_GET_VAL_PRV(std::uint32_t, compute_queue_node_index, std::numeric_limits<std::uint32_t>::max())
    GX_GET_VAL_PRV(std::uint32_t, present_queue_node_index, std::numeric_limits<std::uint32_t>::max())

    constexpr static VkFormat acceptable_depth_formats[] = {
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM,
    };

private:
    [[nodiscard]] int is_good(VkPhysicalDevice gpu) noexcept;

public:
    explicit Physical(std::shared_ptr<Surface> surface) noexcept;
    ~Physical() noexcept;
    [[nodiscard]] std::uint32_t get_memory_type_index(std::uint32_t type_bits, std::uint32_t mem_properties) const noexcept;
};
}
#endif
#endif
