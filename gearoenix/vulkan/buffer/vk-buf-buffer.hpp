#ifndef GEAROENIX_VULKAN_BUFFER_BUFFER_HPP
#define GEAROENIX_VULKAN_BUFFER_BUFFER_HPP
#include "../vk-linker.hpp"
#include <memory>
#include <vector>
namespace gearoenix {
namespace render {
namespace device {
class Logical;
}
namespace command {
class Pool;
}
namespace buffer {
class Buffer {
private:
	std::shared_ptr<device::Logical> logical_device;
	VkBuffer vertices_buffer;
	VkBuffer indices_buffer;
	VkDeviceMemory vertices_memory;
	VkDeviceMemory indices_memory;
    unsigned int indices_count;
public:
	typedef float real;
	Buffer(const std::shared_ptr<command::Pool> &pool,
			const void     *data,    const unsigned int data_size,
			const uint32_t *indices, const unsigned int indices_count);
	~Buffer();
	void map_memory_type_to_index(
			uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex);
	const VkBuffer &get_vertex_buffer() const;
	const VkBuffer &get_index_buffer() const;
    const unsigned int &get_indices_count() const;
};
}
}
}
#endif
