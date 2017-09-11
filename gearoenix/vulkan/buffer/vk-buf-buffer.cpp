#include "vk-buf-buffer.hpp"
#include "../vk-instance.hpp"
#include "../vk-check.hpp"
#include "../device/vk-dev-logical.hpp"
#include "../device/vk-dev-physical.hpp"
#include "../command/vk-cmd-pool.hpp"
#include "../command/vk-cmd-buffer.hpp"
#include "../../core/static.hpp"

gearoenix::nufrag::render::buffer::Buffer::Buffer(
		const std::shared_ptr<command::Pool> &pool,
		const void     *data,    const unsigned int data_size,
		const uint32_t *indices, const unsigned int indices_count):
        logical_device(pool->get_logical_device()), indices_count(indices_count) {
	auto &p = logical_device->get_physical_device();
	auto &l = p->get_instance()->get_linker();
	uint32_t index_buffer_size = indices_count * sizeof(uint32_t);
	VkMemoryAllocateInfo mem_alloc;
	setz(mem_alloc);
	mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	VkMemoryRequirements mem_reqs;
	struct StagingBuffer {
		VkDeviceMemory memory;
		VkBuffer buffer;
	};
	struct {
		StagingBuffer vertices;
		StagingBuffer indices;
	} staging_buffers;
	VkBufferCreateInfo vertex_buffer_info;
	setz(vertex_buffer_info);
	vertex_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vertex_buffer_info.size = data_size;
	vertex_buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	VKC(l->vkCreateBuffer(logical_device->get_vulkan_data(), &vertex_buffer_info, nullptr, &staging_buffers.vertices.buffer));
	l->vkGetBufferMemoryRequirements(logical_device->get_vulkan_data(), staging_buffers.vertices.buffer, &mem_reqs);
	mem_alloc.allocationSize = mem_reqs.size;
	mem_alloc.memoryTypeIndex = p->get_memory_type_index(mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	VKC(l->vkAllocateMemory(logical_device->get_vulkan_data(), &mem_alloc, nullptr, &staging_buffers.vertices.memory));
	void *buffer_data;
	VKC(l->vkMapMemory(logical_device->get_vulkan_data(), staging_buffers.vertices.memory, 0, mem_alloc.allocationSize, 0, &buffer_data));
	std::memcpy(buffer_data, data, data_size);
	l->vkUnmapMemory(logical_device->get_vulkan_data(), staging_buffers.vertices.memory);
	VKC(l->vkBindBufferMemory(logical_device->get_vulkan_data(), staging_buffers.vertices.buffer, staging_buffers.vertices.memory, 0));
	vertex_buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	VKC(l->vkCreateBuffer(logical_device->get_vulkan_data(), &vertex_buffer_info, nullptr, &vertices_buffer));
	l->vkGetBufferMemoryRequirements(logical_device->get_vulkan_data(), vertices_buffer, &mem_reqs);
	mem_alloc.allocationSize = mem_reqs.size;
	mem_alloc.memoryTypeIndex = p->get_memory_type_index(mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VKC(l->vkAllocateMemory(logical_device->get_vulkan_data(), &mem_alloc, nullptr, &vertices_memory));
	VKC(l->vkBindBufferMemory(logical_device->get_vulkan_data(), vertices_buffer, vertices_memory, 0));
	VkBufferCreateInfo index_buffer_info;
	setz(index_buffer_info);
	index_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	index_buffer_info.size = data_size;
	index_buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	VKC(l->vkCreateBuffer(logical_device->get_vulkan_data(), &index_buffer_info, nullptr, &staging_buffers.indices.buffer));
	l->vkGetBufferMemoryRequirements(logical_device->get_vulkan_data(), staging_buffers.indices.buffer, &mem_reqs);
	mem_alloc.allocationSize = mem_reqs.size;
	mem_alloc.memoryTypeIndex = p->get_memory_type_index(mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	VKC(l->vkAllocateMemory(logical_device->get_vulkan_data(), &mem_alloc, nullptr, &staging_buffers.indices.memory));
	VKC(l->vkMapMemory(logical_device->get_vulkan_data(), staging_buffers.indices.memory, 0, data_size, 0, &buffer_data));
	std::memcpy(buffer_data, indices, index_buffer_size);
	l->vkUnmapMemory(logical_device->get_vulkan_data(), staging_buffers.indices.memory);
	VKC(l->vkBindBufferMemory(logical_device->get_vulkan_data(), staging_buffers.indices.buffer, staging_buffers.indices.memory, 0));
	index_buffer_info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	VKC(l->vkCreateBuffer(logical_device->get_vulkan_data(), &index_buffer_info, nullptr, &indices_buffer));
	l->vkGetBufferMemoryRequirements(logical_device->get_vulkan_data(), indices_buffer, &mem_reqs);
	mem_alloc.allocationSize = mem_reqs.size;
	mem_alloc.memoryTypeIndex = p->get_memory_type_index(mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VKC(l->vkAllocateMemory(logical_device->get_vulkan_data(), &mem_alloc, nullptr, &indices_memory));
	VKC(l->vkBindBufferMemory(logical_device->get_vulkan_data(), indices_buffer, indices_memory, 0));
	command::Buffer copy_cmd(pool);
	VkBufferCopy copy_region;
	setz(copy_region);
	copy_region.size = data_size;
	copy_cmd.copy_buffer(staging_buffers.vertices.buffer, vertices_buffer, copy_region);
	copy_region.size = index_buffer_size;
	copy_cmd.copy_buffer(staging_buffers.indices.buffer, indices_buffer, copy_region);
	copy_cmd.flush();
	l->vkDestroyBuffer(logical_device->get_vulkan_data(), staging_buffers.vertices.buffer, nullptr);
	l->vkFreeMemory(logical_device->get_vulkan_data(), staging_buffers.vertices.memory, nullptr);
	l->vkDestroyBuffer(logical_device->get_vulkan_data(), staging_buffers.indices.buffer, nullptr);
	l->vkFreeMemory(logical_device->get_vulkan_data(), staging_buffers.indices.memory, nullptr);
}

gearoenix::nufrag::render::buffer::Buffer::~Buffer() {
	auto &l = logical_device->get_physical_device()->get_instance()->get_linker();
	l->vkDestroyBuffer(logical_device->get_vulkan_data(), vertices_buffer, nullptr);
	l->vkDestroyBuffer(logical_device->get_vulkan_data(), indices_buffer, nullptr);
	l->vkFreeMemory(logical_device->get_vulkan_data(), vertices_memory, nullptr);
	l->vkFreeMemory(logical_device->get_vulkan_data(), indices_memory, nullptr);
}

void gearoenix::nufrag::render::buffer::Buffer::map_memory_type_to_index(
        uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex) {
    VkPhysicalDeviceMemoryProperties memoryProperties;
    auto &l = logical_device->get_physical_device()->get_instance()->get_linker();
    l->vkGetPhysicalDeviceMemoryProperties(
            logical_device->get_physical_device()->get_vulkan_data(), &memoryProperties);
    // Search memtypes to find first index with those properties
    for (uint32_t i = 0; i < 32; i++) {
        if ((typeBits & 1) == 1) {
            // Type is available, does it match user properties?
            if ((memoryProperties.memoryTypes[i].propertyFlags &
                 requirements_mask) == requirements_mask) {
                *typeIndex = i;
            }
        }
        typeBits >>= 1;
    }
}

const VkBuffer &gearoenix::nufrag::render::buffer::Buffer::get_vertex_buffer() const {
	return vertices_buffer;
}

const VkBuffer &gearoenix::nufrag::render::buffer::Buffer::get_index_buffer() const {
	return indices_buffer;
}

const unsigned int &gearoenix::nufrag::render::buffer::Buffer::get_indices_count() const {
    return indices_count;
}
