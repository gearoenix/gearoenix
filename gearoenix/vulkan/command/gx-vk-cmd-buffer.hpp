#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../gx-vk-loader.hpp"
#include "gx-vk-cmd-type.hpp"

#include <memory>
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
private:
    const std::shared_ptr<Pool> pool;
    const Type type;
    vk::raii::CommandBuffer vulkan_data;
    bool has_record = false;

public:
    Buffer(std::shared_ptr<Pool>&& p, Type t);
    Buffer(Buffer&&) noexcept;
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    ~Buffer();
    void begin();
    void end();
    void copy(const buffer::Buffer& src, const buffer::Buffer& des, const std::vector<vk::BufferCopy>&);
    void copy(const buffer::Buffer& src, const buffer::Buffer& des);
    void barrier(const buffer::Buffer& buff, std::pair<vk::AccessFlags, vk::PipelineStageFlags> src_state, std::pair<vk::AccessFlags, vk::PipelineStageFlags> des_state);
    void set(const Viewport& viewport);
    void set(const Scissor& scissor);
    void bind(Pipeline pip);
    void bind(const DescriptorSet& set);
    void bind_vertices(buffer::Buffer& buf);
    void bind_indices(buffer::Buffer& buf);
    void draw_indices(std::uint64_t count);

    [[nodiscard]] const std::shared_ptr<Pool>& get_pool() const { return pool; }
    [[nodiscard]] Type get_type() const { return type; }
    [[nodiscard]] const vk::raii::CommandBuffer& get_command_buffer() const { return vulkan_data; }
    [[nodiscard]] vk::CommandBuffer get_vulkan_data() const { return *vulkan_data; }
    [[nodiscard]] bool get_has_record() const { return has_record; }
    void set_has_record(bool v) { has_record = v; }
};
}
#endif
