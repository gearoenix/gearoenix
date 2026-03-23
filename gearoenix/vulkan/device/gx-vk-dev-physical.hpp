#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-singleton.hpp"
#include "../gx-vk-loader.hpp"

#include <array>
#include <limits>
#include <optional>
#include <set>
#include <string>
#include <vector>

namespace gearoenix::vulkan {
struct Instance;
struct Surface;
}

namespace gearoenix::vulkan::device {
struct Physical final : core::Singleton<Physical> {
    constexpr static std::array<vk::Format, 5> acceptable_depth_formats = {
        vk::Format::eD32SfloatS8Uint,
        vk::Format::eD32Sfloat,
        vk::Format::eD24UnormS8Uint,
        vk::Format::eD16UnormS8Uint,
        vk::Format::eD16Unorm,
    };

private:
    vk::raii::PhysicalDevice vulkan_data;
    std::set<std::string> supported_extensions;
    vk::PhysicalDeviceFeatures features = { };
    vk::PhysicalDeviceRayQueryFeaturesKHR ray_query_features = { };
    vk::PhysicalDeviceRayTracingPipelineFeaturesKHR ray_tracing_pipeline_features = { };
    vk::PhysicalDeviceShaderClockFeaturesKHR shader_clock_features = { };
    vk::PhysicalDeviceVulkan12Features vulkan_12_features = { };
    vk::PhysicalDeviceVulkan13Features vulkan_13_features = { };
    vk::PhysicalDeviceDynamicRenderingFeatures dynamic_rendering_features = { };
    vk::PhysicalDeviceDescriptorIndexingProperties descriptor_indexing_properties = { };
    vk::PhysicalDeviceProperties properties = { };
    vk::PhysicalDeviceMemoryProperties memory_properties = { };
    std::vector<vk::QueueFamilyProperties> queue_family_properties;
    vk::PhysicalDeviceRayTracingPipelinePropertiesKHR ray_tracing_pipeline_properties = { };
    std::vector<vk::SurfaceFormatKHR> surface_formats;
    vk::Format supported_depth_format = vk::Format::eUndefined;
    std::uint32_t max_memory_alignment = std::numeric_limits<std::uint32_t>::max();
    std::uint32_t graphics_queue_node_index = std::numeric_limits<std::uint32_t>::max();
    std::uint32_t transfer_queue_node_index = std::numeric_limits<std::uint32_t>::max();
    std::uint32_t compute_queue_node_index = std::numeric_limits<std::uint32_t>::max();
    std::uint32_t present_queue_node_index = std::numeric_limits<std::uint32_t>::max();
    bool rtx_supported = false;

    [[nodiscard]] int is_good(const vk::raii::PhysicalDevice& gpu);
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

    [[nodiscard]] const vk::raii::PhysicalDevice& get_physical_device() const { return vulkan_data; }
    [[nodiscard]] vk::PhysicalDevice get_vulkan_data() const { return *vulkan_data; }
    [[nodiscard]] const std::set<std::string>& get_supported_extensions() const { return supported_extensions; }
    [[nodiscard]] const vk::PhysicalDeviceFeatures& get_features() const { return features; }
    [[nodiscard]] const vk::PhysicalDeviceRayQueryFeaturesKHR& get_ray_query_features() const { return ray_query_features; }
    [[nodiscard]] const vk::PhysicalDeviceRayTracingPipelineFeaturesKHR& get_ray_tracing_pipeline_features() const { return ray_tracing_pipeline_features; }
    [[nodiscard]] const vk::PhysicalDeviceShaderClockFeaturesKHR& get_shader_clock_features() const { return shader_clock_features; }
    [[nodiscard]] const vk::PhysicalDeviceVulkan12Features& get_vulkan_12_features() const { return vulkan_12_features; }
    [[nodiscard]] const vk::PhysicalDeviceVulkan13Features& get_vulkan_13_features() const { return vulkan_13_features; }
    [[nodiscard]] const vk::PhysicalDeviceDynamicRenderingFeatures& get_dynamic_rendering_features() const { return dynamic_rendering_features; }
    [[nodiscard]] const vk::PhysicalDeviceDescriptorIndexingProperties& get_descriptor_indexing_properties() const { return descriptor_indexing_properties; }
    [[nodiscard]] const vk::PhysicalDeviceProperties& get_properties() const { return properties; }
    [[nodiscard]] const vk::PhysicalDeviceMemoryProperties& get_memory_properties() const { return memory_properties; }
    [[nodiscard]] const std::vector<vk::QueueFamilyProperties>& get_queue_family_properties() const { return queue_family_properties; }
    [[nodiscard]] const vk::PhysicalDeviceRayTracingPipelinePropertiesKHR& get_ray_tracing_pipeline_properties() const { return ray_tracing_pipeline_properties; }
    [[nodiscard]] const std::vector<vk::SurfaceFormatKHR>& get_surface_formats() const { return surface_formats; }
    [[nodiscard]] vk::Format get_supported_depth_format() const { return supported_depth_format; }
    [[nodiscard]] std::uint32_t get_max_memory_alignment() const { return max_memory_alignment; }
    [[nodiscard]] std::uint32_t get_graphics_queue_node_index() const { return graphics_queue_node_index; }
    [[nodiscard]] std::uint32_t get_transfer_queue_node_index() const { return transfer_queue_node_index; }
    [[nodiscard]] std::uint32_t get_compute_queue_node_index() const { return compute_queue_node_index; }
    [[nodiscard]] std::uint32_t get_present_queue_node_index() const { return present_queue_node_index; }
    [[nodiscard]] bool get_rtx_supported() const { return rtx_supported; }

    [[nodiscard]] std::optional<std::uint32_t> get_memory_type_index(std::uint32_t type_bits, vk::MemoryPropertyFlags mem_properties) const;
    [[nodiscard]] vk::SurfaceCapabilitiesKHR get_surface_capabilities() const;
};
}
#endif
