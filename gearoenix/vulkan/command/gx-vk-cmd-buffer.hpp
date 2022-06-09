#ifndef GEAROENIX_VULKAN_COMMAND_BUFFER_HPP
#define GEAROENIX_VULKAN_COMMAND_BUFFER_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"
#include "gx-vk-cmd-type.hpp"
#include <vector>

namespace gearoenix::vulkan {
struct DescriptorSet;
struct Framebuffer;
struct Pipeline;
struct RenderPass;
struct Scissor;
struct Viewport;
}

namespace gearoenix::vulkan::buffer {
struct Buffer;
}

namespace gearoenix::vulkan::command {
struct Pool;
struct Buffer final {
    GX_GET_PTRC_PRV(Pool, pool)
    GX_GET_CVAL_PRV(Type, type)
    GX_GET_VAL_PRV(VkCommandBuffer, vulkan_data, nullptr)

public:
    Buffer(Pool* pool, Type t) noexcept;
    Buffer(const Buffer&) noexcept = delete;
    Buffer& operator=(const Buffer&) noexcept = delete;
    Buffer(Buffer&&) noexcept;
    ~Buffer() noexcept;
    void begin() noexcept;
    void end() noexcept;
    void copy(buffer::Buffer& src, buffer::Buffer& des, const std::vector<VkBufferCopy>&) noexcept;
    void copy(buffer::Buffer& src, buffer::Buffer& des) noexcept;
    void barrier(
        buffer::Buffer& buff,
        std::pair<VkAccessFlags, VkPipelineStageFlags> src_state,
        std::pair<VkAccessFlags, VkPipelineStageFlags> des_state) noexcept;
    void begin(const RenderPass& render_pass, const Framebuffer& framebuffer) noexcept;
    void end_render_pass() noexcept;
    void set(const Viewport& viewport) noexcept;
    void set(const Scissor& scissor) noexcept;
    void bind(Pipeline pip) noexcept;
    void bind(const DescriptorSet& set) noexcept;
    void bind_vertices(buffer::Buffer& buf) noexcept;
    void bind_indices(buffer::Buffer& buf) noexcept;
    void draw_indices(std::size_t count) noexcept;
    void build_acceleration_structure(
        const VkAccelerationStructureBuildGeometryInfoKHR&,
        const VkAccelerationStructureBuildRangeInfoKHR* const* const) noexcept;
    [[nodiscard]] const VkCommandBuffer* get_vulkan_data_ptr() const noexcept;
};
}
#endif
#endif
