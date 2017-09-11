#ifndef GEAROENIX_VULKAN_BUFFER_UNIFORM_HPP
#define GEAROENIX_VULKAN_BUFFER_UNIFORM_HPP
#include <memory>
#include "../vk-linker.hpp"
namespace gearoenix {
namespace render {
namespace command {
class Pool;
}
namespace device {
class Logical;
}
namespace memory {
class Memory;
class Pool;
}
namespace buffer {
class Uniform
{
private:
	std::shared_ptr<device::Logical> logical_device;
	std::shared_ptr<memory::Memory> mem;
	VkBuffer vulkan_data;
	VkDeviceSize data_size;
public:
	Uniform(std::shared_ptr<memory::Pool> &pool, int data_size);
	~Uniform();
	void update(const void *data);
    const std::shared_ptr<device::Logical> &get_logical_device() const;
    const std::shared_ptr<memory::Memory> &get_memory() const;
    const VkBuffer &get_vulkan_data() const;
    const VkDeviceSize &get_data_size() const;
};
} // namespace buffer
} // namespace render
} // namespace gearoenix
#endif // GEAROENIX_RENDER_BUFFER_UNIFORM_HPP
