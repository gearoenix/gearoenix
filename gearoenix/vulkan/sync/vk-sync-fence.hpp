#ifndef GEAROENIX_NUFRAG_VULKAN_FENCE_HPP
#define GEAROENIX_NUFRAG_VULKAN_FENCE_HPP
#include <memory>
#include "../vk-linker.hpp"
namespace gearoenix {
namespace nufrag {
namespace render {
namespace device {
class Logical;
}
namespace sync {
class Fence {
private:
	std::shared_ptr<device::Logical> logical_device;
    VkFence vulkan_data;
public:
	Fence(const std::shared_ptr<device::Logical> &logical_device, bool signaled = false);
	~Fence();
	void wait();
	const VkFence &get_vulkan_data() const;
};
}
}
}
}
#endif
