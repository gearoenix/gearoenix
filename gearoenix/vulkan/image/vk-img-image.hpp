#ifndef GEAROENIX_VULKAN_IMAGE_IMAGE_HPP
#define GEAROENIX_VULKAN_IMAGE_IMAGE_HPP
#include "../vk-linker.hpp"
#include <memory>
namespace gearoenix {
namespace render {
namespace device {
class Logical;
}
namespace memory {
class Pool;
class Memory;
}
namespace image {
class Image {
private:
	std::shared_ptr<device::Logical> logical_device;
	VkImage vulkan_data;
    std::shared_ptr<memory::Memory> mem;
public:
    Image(const std::shared_ptr<device::Logical> &logical_device, const VkImage &vulkan_data, const std::shared_ptr<memory::Memory> &mem = nullptr);
    Image(const std::shared_ptr<device::Logical> &logical_device, const VkImageCreateInfo &info, const std::shared_ptr<memory::Pool> &pool);
	~Image();
	const VkImage &get_vulkan_data() const;
	const std::shared_ptr<device::Logical> &get_logical_device() const;
};
}
}
}
#endif
