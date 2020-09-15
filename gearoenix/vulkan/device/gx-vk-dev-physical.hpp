#ifndef GEAROENIX_VULKAN_DEVICE_PHYSICAL_HPP
#define GEAROENIX_VULKAN_DEVICE_PHYSICAL_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef USE_VULKAN
#include "../gx-vk-linker.hpp"
#include <memory>
#include <vector>
namespace gearoenix {
namespace render {
    class Instance;
    class Surface;
    namespace device {
        class Physical {
            Surface* surface;
            VkPhysicalDevice vulkan_data;
            VkPhysicalDeviceProperties properties;
            VkPhysicalDeviceFeatures features;
            VkPhysicalDeviceMemoryProperties memory_properties;
            VkSurfaceCapabilitiesKHR surface_capabilities;
            std::vector<VkQueueFamilyProperties> queue_family_properties;
            std::vector<std::string> supported_extensions;
            uint32_t graphics_queue_node_index = UINT32_MAX;
            uint32_t transfer_queue_node_index = UINT32_MAX;
            uint32_t compute_queue_node_index = UINT32_MAX;
            uint32_t present_queue_node_index = UINT32_MAX;
            int is_good(const VkPhysicalDevice& gpu);

        public:
            Physical(Surface* surface);
            ~Physical();
            const VkPhysicalDeviceMemoryProperties& get_memory_properties() const;
            const VkSurfaceCapabilitiesKHR& get_surface_capabilities() const;
            std::vector<VkSurfaceFormatKHR> get_surface_formats() const;
            const Surface* get_surface() const;
            const Instance* get_instance() const;
            Instance* get_instance();
            const VkPhysicalDevice& get_vulkan_data() const;
            uint32_t get_graphics_queue_node_index() const;
            uint32_t get_transfer_queue_node_index() const;
            uint32_t get_compute_queue_node_index() const;
            uint32_t get_present_queue_node_index() const;
            VkFormat get_supported_depth_format() const;
            uint32_t get_memory_type_index(uint32_t type_bits, uint32_t properties) const;
            unsigned int get_max_memory_alignment() const;
            const VkPhysicalDeviceProperties& get_properties() const;
        };
    }
}
}
#endif
#endif
