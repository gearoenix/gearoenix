#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
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
    GX_GET_PTRC_PRV(Pool, pool);
    GX_GET_CVAL_PRV(Type, type);
    GX_GET_VAL_PRV(VkCommandBuffer, vulkan_data, nullptr);
    GX_GETSET_VAL_PRV(bool, has_record, false);

public:
    Buffer(Pool* pool, Type t);
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    Buffer(Buffer&&);
    ~Buffer();
    void begin();
    void end();
    void copy(buffer::Buffer& src, buffer::Buffer& des, const std::vector<VkBufferCopy>&);
    void copy(buffer::Buffer& src, buffer::Buffer& des);
    void barrier(
        buffer::Buffer& buff,
        std::pair<VkAccessFlags, VkPipelineStageFlags> src_state,
        std::pair<VkAccessFlags, VkPipelineStageFlags> des_state);
    void begin(const RenderPass& render_pass, const Framebuffer& framebuffer);
    void end_render_pass();
    void set(const Viewport& viewport);
    void set(const Scissor& scissor);
    void bind(Pipeline pip);
    void bind(const DescriptorSet& set);
    void bind_vertices(buffer::Buffer& buf);
    void bind_indices(buffer::Buffer& buf);
    void draw_indices(std::uint64_t count);
    void build_acceleration_structure(
        const VkAccelerationStructureBuildGeometryInfoKHR&,
        const VkAccelerationStructureBuildRangeInfoKHR* const* const);
    [[nodiscard]] const VkCommandBuffer* get_vulkan_data_ptr() const;
};
}
#endif
