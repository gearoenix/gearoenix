#include "gx-vk-msh-mesh.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/allocator/gx-cr-alc-shared-array.hpp"
#include "gx-vk-msh-buffer.hpp"

namespace {
const auto mesh_allocator = gearoenix::core::allocator::SharedArray<gearoenix::vulkan::mesh::Mesh, gearoenix::render::mesh::Mesh::max_count>::construct();
}

gearoenix::vulkan::mesh::Mesh::Mesh(
    std::shared_ptr<render::mesh::Buffer>&& buffer,
    std::shared_ptr<render::material::Material>&& material)
    : render::mesh::Mesh(std::move(buffer), std::move(material))
    , gapi_buffer(std::dynamic_pointer_cast<Buffer>(this->buffer))
{
}

gearoenix::vulkan::mesh::Mesh::~Mesh() = default;

void gearoenix::vulkan::mesh::Mesh::construct(
    std::shared_ptr<render::mesh::Buffer>&& buffer,
    std::shared_ptr<render::material::Material>&& material,
    const core::job::EndCallerShared<render::mesh::Mesh>& end_callback)
{
    end_callback.set_return(mesh_allocator->make_shared(std::move(buffer), std::move(material)));
}

#endif