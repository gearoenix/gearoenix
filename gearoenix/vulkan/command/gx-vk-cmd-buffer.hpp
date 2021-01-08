#ifndef GEAROENIX_VULKAN_COMMAND_BUFFER_HPP
#define GEAROENIX_VULKAN_COMMAND_BUFFER_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-vk-loader.hpp"

namespace gearoenix::vulkan::buffer {
struct Buffer;
}

namespace gearoenix::vulkan {
struct RenderPass;
struct Viewport;
struct Scissor;
struct Pipeline;
struct DescriptorSet;
}

namespace gearoenix::vulkan::command {
struct Pool;
struct Buffer final {
    GX_GET_PTRC_PRV(Pool, pool)
    GX_GET_VAL_PRV(VkCommandBuffer, vulkan_data, nullptr)

    Buffer(Pool* const pool, VkCommandBuffer vulkan_data) noexcept;

public:
    Buffer(const Buffer&) noexcept = delete;
    Buffer& operator=(const Buffer&) noexcept = delete;
    Buffer(Buffer&&) noexcept = default;
    ~Buffer() noexcept;
    [[nodiscard]] static Buffer create(Pool* pool, bool is_primary) noexcept;
    [[nodiscard]] static Buffer create_primary(Pool* pool) noexcept;
    [[nodiscard]] static Buffer create_secondary(Pool* pool) noexcept;
    void begin() noexcept;
    void flush() noexcept;
    void end() noexcept;
    void copy(buffer::Buffer& src, buffer::Buffer& des) noexcept;
    void begin(const RenderPass& render_pass) noexcept;
    void end_render_pass() noexcept;
    void set(const Viewport& viewport) noexcept;
    void set(const Scissor& scissor) noexcept;
    void bind(Pipeline pip) noexcept;
    void bind(const DescriptorSet& set) noexcept;
    void bind_vertex_buffers(buffer::Buffer& buf) noexcept;
    void bind_index_buffer(buffer::Buffer& buf) noexcept;
    void draw_indexed(std::size_t count) noexcept;
};
}
#endif
#endif
