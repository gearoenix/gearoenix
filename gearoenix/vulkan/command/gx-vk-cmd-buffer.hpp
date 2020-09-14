#ifndef GEAROENIX_VULKAN_COMMAND_BUFFER_HPP
#define GEAROENIX_VULKAN_COMMAND_BUFFER_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../gx-vk-loader.hpp"
#include <memory>
#include <vector>

namespace gearoenix::render::command {
class Pool;
class Buffer {
private:
    const std::shared_ptr<Pool> pool;
    VkCommandBuffer vulkan_data {};

public:
    explicit Buffer(std::shared_ptr<Pool> pool) noexcept;
    ~Buffer() noexcept;
    void begin() noexcept;
    void reset() noexcept;
    void copy_buffer(VkBuffer src, VkBuffer des, const VkBufferCopy& region) noexcept;
    void flush() noexcept;
    [[nodiscard]] const VkCommandBuffer& get_vulkan_data() const noexcept;
    void begin_render_pass_with_info(const VkRenderPassBeginInfo& render_pass_begin_info) noexcept;
    void set_viewport(const VkViewport& viewport) noexcept;
    void set_scissor(const VkRect2D& scissor) noexcept;
    void end() noexcept;
    void end_render_pass() noexcept;
    void bind_pipeline(VkPipeline pip) noexcept;
    void bind_descriptor_set(
        VkPipelineBindPoint pipeline_bind_point,
        VkPipelineLayout pipeline_layout,
        uint32_t first_set,
        const VkDescriptorSet& des_set) noexcept;
    void bind_vertex_buffers(VkBuffer buf, VkDeviceSize offset) noexcept;
    void bind_index_buffer(VkBuffer buf, VkDeviceSize offset) noexcept;
    void draw_indexed(VkDeviceSize count) noexcept;
};
}
#endif
#endif
