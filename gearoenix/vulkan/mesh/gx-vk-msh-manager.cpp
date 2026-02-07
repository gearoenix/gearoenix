#include "gx-vk-msh-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "gx-vk-msh-buffer.hpp"
#include "gx-vk-msh-mesh.hpp"

gearoenix::vulkan::mesh::Manager::Manager()
    : Singleton<Manager>(this)
{
    core::Object::register_type<Mesh>();
}

gearoenix::vulkan::mesh::Manager::~Manager() = default;

void gearoenix::vulkan::mesh::Manager::build(std::string&& name, render::Vertices&& vertices, std::vector<std::uint32_t>&& indices, const math::Aabb3<double>& occlusion_box, core::job::EndCallerShared<render::mesh::Buffer>&& end_callback)
{
    Buffer::construct(std::move(name), std::move(vertices), std::move(indices), occlusion_box, std::move(end_callback));
}

void gearoenix::vulkan::mesh::Manager::build(
    std::string&& name,
    std::shared_ptr<render::mesh::Buffer>&& buffer,
    std::shared_ptr<render::material::Material>&& material,
    core::job::EndCallerShared<render::mesh::Mesh>&& end_callback)
{
    Mesh::construct(std::move(name), std::move(buffer), std::move(material), end_callback);
}

#endif
