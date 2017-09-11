#ifndef GEAROENIX_NUFRAG_VULKAN_COMMAND_BUFFER_HPP
#define GEAROENIX_NUFRAG_VULKAN_COMMAND_BUFFER_HPP
#include "../vk-linker.hpp"
#include <memory>
#include <vector>
namespace gearoenix {
namespace nufrag {
namespace render {
namespace command {
class Pool;
class Buffer {
private:
	std::shared_ptr<Pool> pool;
	std::shared_ptr<Linker> linker;
	VkCommandBuffer vulkan_data;
	bool not_flushed = false;
public:
	Buffer(const std::shared_ptr<Pool> &pool);
	~Buffer();
	void copy_buffer(VkBuffer src, VkBuffer des, const VkBufferCopy &region);
	void flush();
	const VkCommandBuffer &get_vulkan_data() const;
    void begin_render_pass_with_info(const VkRenderPassBeginInfo &render_pass_begin_info);
    void set_viewport(const VkViewport &viewport);
    void set_scissor(const VkRect2D &scissor);
};
}
}
}
}
#endif
