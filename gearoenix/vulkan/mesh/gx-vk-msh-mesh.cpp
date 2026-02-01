#include "gx-vk-msh-mesh.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-object.hpp"
#include "gx-vk-msh-buffer.hpp"

gearoenix::vulkan::mesh::Mesh::Mesh(
    std::string&& name,
    std::shared_ptr<render::mesh::Buffer>&& buffer,
    std::shared_ptr<render::material::Material>&& material)
    : render::mesh::Mesh(std::move(name), std::move(buffer), std::move(material))
    , gapi_buffer(std::dynamic_pointer_cast<Buffer>(this->buffer))
{
}

gearoenix::vulkan::mesh::Mesh::~Mesh() = default;

void gearoenix::vulkan::mesh::Mesh::construct(
    std::string&& name,
    std::shared_ptr<render::mesh::Buffer>&& buffer,
    std::shared_ptr<render::material::Material>&& material,
    const core::job::EndCallerShared<render::mesh::Mesh>& end_callback)
{
    end_callback.set_return(Object::construct<Mesh>(std::move(name), std::move(buffer), std::move(material)));
}

#endif