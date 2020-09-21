#ifndef GEAROENIX_VULKAN_COMMAND_BUFFER_HPP
#define GEAROENIX_VULKAN_COMMAND_BUFFER_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../../render/command/gx-rnd-cmd-buffer.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan::command {
class Pool;
class Buffer final : public render::command::Buffer {
private:
    GX_GET_REFC_PRV(std::shared_ptr<Pool>, pool)
    GX_GET_VAL_PRV(VkCommandBuffer, vulkan_data, nullptr)

public:
    Buffer(std::shared_ptr<Pool> pool, render::command::Type command_buffer_type) noexcept;
    ~Buffer() noexcept final;
    void begin() noexcept final;
    void copy_buffer(VkBuffer src, VkBuffer des, const VkBufferCopy& region) noexcept;
    void flush() noexcept;
    void begin_render_pass_with_info(const VkRenderPassBeginInfo& render_pass_begin_info) noexcept;
    void set_viewport(const VkViewport& viewport) noexcept;
    void set_scissor(const VkRect2D& scissor) noexcept;
    void end() noexcept final;
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
