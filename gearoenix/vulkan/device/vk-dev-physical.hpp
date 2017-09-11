#ifndef GEAROENIX_VULKAN_DEVICE_PHYSICAL_HPP
#define GEAROENIX_VULKAN_DEVICE_PHYSICAL_HPP
#include "../vk-linker.hpp"
#include <memory>
#include <vector>
namespace gearoenix {
namespace render {
class Instance;
class Surface;
namespace device {
class Physical {
	std::shared_ptr<Surface> surface;
	VkPhysicalDevice vulkan_data;
	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceFeatures features;
	VkPhysicalDeviceMemoryProperties memory_properties;
	std::vector<VkQueueFamilyProperties> queue_family_properties;
	std::vector<std::string> supported_extensions;
	uint32_t graphics_queue_node_index = UINT32_MAX;
	uint32_t transfer_queue_node_index = UINT32_MAX;
	uint32_t compute_queue_node_index  = UINT32_MAX;
	uint32_t present_queue_node_index  = UINT32_MAX;
	int is_good(const VkPhysicalDevice &gpu);
public:
	Physical(const std::shared_ptr<Surface> &surface);
	~Physical();
	const VkPhysicalDeviceMemoryProperties &get_memory_properties() const;
	std::shared_ptr<VkSurfaceCapabilitiesKHR> get_surface_capabilities() const;
	std::vector<VkSurfaceFormatKHR> get_surface_formats() const;
	const std::shared_ptr<Surface> &get_surface() const;
	const std::shared_ptr<Instance> &get_instance() const;
	const VkPhysicalDevice &get_vulkan_data() const;
	uint32_t get_graphics_queue_node_index() const;
	uint32_t get_transfer_queue_node_index() const;
	uint32_t get_compute_queue_node_index() const;
	uint32_t get_present_queue_node_index() const;
	VkFormat get_supported_depth_format() const;
	uint32_t get_memory_type_index(uint32_t type_bits, uint32_t properties) const;
};
}
}
}
#endif
