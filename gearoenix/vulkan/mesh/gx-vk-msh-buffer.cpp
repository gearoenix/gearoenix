#include "gx-vk-msh-buffer.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/allocator/gx-cr-alc-shared-array.hpp"
#include "../../render/gx-rnd-vertex.hpp"
#include "../buffer/gx-vk-buf-manager.hpp"

namespace {
const auto buffer_allocator = gearoenix::core::allocator::SharedArray<gearoenix::vulkan::mesh::Buffer, gearoenix::render::mesh::Buffer::max_count>::construct();
}

gearoenix::vulkan::mesh::Buffer::Buffer(const math::Aabb3<double>& box)
    : render::mesh::Buffer(box)
{
}

gearoenix::vulkan::mesh::Buffer::~Buffer() = default;

void gearoenix::vulkan::mesh::Buffer::construct(std::string&& name, render::Vertices&& vertices, std::vector<std::uint32_t>&& indices, const math::Aabb3<double>& occlusion_box, core::job::EndCallerShared<render::mesh::Buffer>&& end_callback)
{
    auto result = buffer_allocator->make_shared(occlusion_box);
    result->indices_count = static_cast<std::uint32_t>(indices.size());

    core::job::EndCaller end([c = std::move(end_callback), r = result] { c.set_return(r); });

    auto& buf_mgr = buffer::Manager::get();
    result->vertex = buf_mgr.create(name + "-vertex", render::get_data(vertices), static_cast<std::int64_t>(core::bytes_count(vertices)), core::job::EndCaller(end));
    result->index = buf_mgr.create(name + "-index", indices.data(), static_cast<std::int64_t>(indices.size() * sizeof(std::uint32_t)), std::move(end));
}

#endif