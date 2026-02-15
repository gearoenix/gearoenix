#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "../../render/gx-rnd-vertex.hpp"
#include "../../render/mesh/gx-rnd-msh-buffer.hpp"

namespace gearoenix::vulkan::buffer {
struct Buffer;
}

namespace gearoenix::vulkan::mesh {
struct Buffer final : render::mesh::Buffer {
    GX_GET_CREF_PRV(std::shared_ptr<buffer::Buffer>, vertex);
    GX_GET_CREF_PRV(std::shared_ptr<buffer::Buffer>, index);
    GX_GET_VAL_PRV(std::uint32_t, indices_count, 0);

public:
    explicit Buffer(const math::Aabb3<double>& box);
    static void construct(std::string&& name, render::Vertices&& vertices, std::vector<std::uint32_t>&& indices, const math::Aabb3<double>& occlusion_box, core::job::EndCallerShared<render::mesh::Buffer>&& end_callback);
    ~Buffer() override;
};
}

#endif