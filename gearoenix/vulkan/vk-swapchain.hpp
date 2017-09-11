#ifndef GEAROENIX_NUFRAG_VULKAN_SWAPCHAIN_HPP
#define GEAROENIX_NUFRAG_VULKAN_SWAPCHAIN_HPP
#include <memory>
#include <vector>
#include "vk-linker.hpp"
namespace gearoenix {
namespace nufrag {
namespace render {
namespace device {
class Logical;
}
namespace image {
class View;
}
namespace sync {
class Semaphore;
}
class Swapchain {
private:
	std::shared_ptr<device::Logical> logical_device;
	VkSurfaceFormatKHR chosen_format;
    VkSwapchainKHR vulkan_data = 0;
	std::vector<std::shared_ptr<image::View> > image_views;
public:
	Swapchain(const std::shared_ptr<device::Logical> &d);
	~Swapchain();
	const VkSwapchainKHR &get_vulkan_data() const;
	const std::shared_ptr<device::Logical> &get_logical_device() const;
	const VkSurfaceFormatKHR &get_chosen_format() const;
	std::vector<std::shared_ptr<image::View> > get_image_views() const;
    uint32_t get_next_image_index(const std::shared_ptr<sync::Semaphore> &semaphore);
    void initialize();
};
}
}
}
#endif
