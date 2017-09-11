#ifndef GEAROENIX_NUFRAG_VULKAN_DEVICE_LOGICAL_HPP
#define GEAROENIX_NUFRAG_VULKAN_DEVICE_LOGICAL_HPP
#include "../vk-linker.hpp"
#include <memory>
namespace gearoenix {
namespace nufrag {
namespace render {
namespace device {
class Physical;
class Logical {
private:
	std::shared_ptr<Physical> physical_device;
	VkDevice vulkan_data;
	VkQueue vk_graphic_queue;
public:
	Logical(const std::shared_ptr<Physical> &p);
	~Logical();
	const std::shared_ptr<Physical> &get_physical_device() const;
	const VkDevice &get_vulkan_data() const;
	const VkQueue &get_graphic_queue() const;
    void wait_to_finish();
};
}
}
}
}
#endif
