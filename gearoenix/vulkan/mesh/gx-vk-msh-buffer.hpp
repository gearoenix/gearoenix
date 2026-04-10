#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "../../render/gx-rnd-vertex.hpp"
#include "../../render/mesh/gx-rnd-msh-buffer.hpp"

namespace gearoenix::vulkan::buffer {
struct Buffer;
}

namespace gearoenix::vulkan::mesh {
struct Buffer final : render::mesh::Buffer {
private:
    std::shared_ptr<buffer::Buffer> vertex;
    std::shared_ptr<buffer::Buffer> index;
    std::uint32_t indices_count = 0;

public:
    [[nodiscard]] const std::shared_ptr<buffer::Buffer>& get_vertex() const { return vertex; }
    [[nodiscard]] const std::shared_ptr<buffer::Buffer>& get_index() const { return index; }
    [[nodiscard]] std::uint32_t get_indices_count() const { return indices_count; }

    explicit Buffer(const math::Aabb3<core::fp_t>& box);
    static void construct(std::string&& name, render::Vertices&& vertices, std::vector<std::uint32_t>&& indices, const math::Aabb3<core::fp_t>& occlusion_box, core::job::EndCallerShared<render::mesh::Buffer>&& end_callback);
    ~Buffer() override;
};
}

#endif