#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../core/gx-cr-singleton.hpp"
#include "../gx-vk-loader.hpp"

#include <limits>
#include <set>
#include <string>
#include <vector>
#include <array>

namespace gearoenix::vulkan {
struct Instance;
struct Surface;
}

namespace gearoenix::vulkan::device {
struct Physical final : core::Singleton<Physical> {
    constexpr static std::array<VkFormat, 5> acceptable_depth_formats = {
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM,
    };

    GX_GET_VAL_PRV(VkPhysicalDevice, vulkan_data, nullptr);
    GX_GET_CREF_PRV(std::set<std::string>, supported_extensions);
    GX_GET_CREF_PRV(VkPhysicalDeviceFeatures, features);
    GX_GET_CREF_PRV(VkPhysicalDeviceRayQueryFeaturesKHR, ray_query_features);
    GX_GET_CREF_PRV(VkPhysicalDeviceRayTracingPipelineFeaturesKHR, ray_tracing_pipeline_features);
    GX_GET_CREF_PRV(VkPhysicalDeviceShaderClockFeaturesKHR, shader_clock_features);
    GX_GET_CREF_PRV(VkPhysicalDeviceVulkan12Features, vulkan_12_features);
    GX_GET_CREF_PRV(VkPhysicalDeviceVulkan13Features, vulkan_13_features);
    GX_GET_CREF_PRV(VkPhysicalDeviceDynamicRenderingFeatures, dynamic_rendering_features);
    GX_GET_CREF_PRV(VkPhysicalDeviceDescriptorIndexingProperties, descriptor_indexing_properties);
    GX_GET_CREF_PRV(VkPhysicalDeviceProperties, properties);
    GX_GET_CREF_PRV(VkPhysicalDeviceMemoryProperties, memory_properties);
    GX_GET_CREF_PRV(std::vector<VkQueueFamilyProperties>, queue_family_properties);
    GX_GET_CREF_PRV(VkPhysicalDeviceRayTracingPipelinePropertiesKHR, ray_tracing_pipeline_properties);
    GX_GET_CREF_PRV(std::vector<VkSurfaceFormatKHR>, surface_formats);
    GX_GET_VAL_PRV(VkFormat, supported_depth_format, VK_FORMAT_UNDEFINED);
    GX_GET_VAL_PRV(std::uint32_t, max_memory_alignment, std::numeric_limits<std::uint32_t>::max());
    GX_GET_VAL_PRV(std::uint32_t, graphics_queue_node_index, std::numeric_limits<std::uint32_t>::max());
    GX_GET_VAL_PRV(std::uint32_t, transfer_queue_node_index, std::numeric_limits<std::uint32_t>::max());
    GX_GET_VAL_PRV(std::uint32_t, compute_queue_node_index, std::numeric_limits<std::uint32_t>::max());
    GX_GET_VAL_PRV(std::uint32_t, present_queue_node_index, std::numeric_limits<std::uint32_t>::max());
    GX_GET_VAL_PRV(bool, rtx_supported, false);

    [[nodiscard]] int is_good(VkPhysicalDevice gpu);
    void initialize_extensions();
    void initialize_features();
    void initialize_properties();

public:
    Physical();
    Physical(Physical&&) = delete;
    Physical(const Physical&) = delete;
    Physical& operator=(Physical&&) = delete;
    Physical& operator=(const Physical&) = delete;
    ~Physical() override;

    [[nodiscard]] std::uint32_t get_memory_type_index(std::uint32_t type_bits, std::uint32_t mem_properties) const;
    [[nodiscard]] std::uint64_t align_size(std::uint64_t size) const;

    /// It's going to fetch new information about the surface every time.
    ///
    /// \note If you don't care about new information you can cache it for your future uses.
    [[nodiscard]] VkSurfaceCapabilitiesKHR get_surface_capabilities() const;
    [[nodiscard]] static std::vector<VkPhysicalDevice> get_available_devices(VkInstance instance);
};
}
#endif