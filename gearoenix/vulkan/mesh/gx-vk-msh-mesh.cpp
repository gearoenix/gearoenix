#include "gx-vk-msh-mesh.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../engine/gx-vk-eng-engine.hpp"

gearoenix::vulkan::mesh::Mesh::Mesh(
    engine::Engine& e,
    std::vector<math::BasicVertex> vertices,
    std::vector<std::uint32_t> indices) noexcept
    : core::ecs::Component(this)
    , render::mesh::Mesh()
    , vertex(*e.get_buffer_manager().create(vertices))
    , index(*e.get_buffer_manager().create(indices))
{
    vertices.clear();
    indices.clear();
}

gearoenix::vulkan::mesh::Mesh::Mesh(Mesh&&) noexcept = default;

std::shared_ptr<gearoenix::vulkan::mesh::Mesh> gearoenix::vulkan::mesh::Mesh::construct(
    engine::Engine& e,
    std::vector<math::BasicVertex> vertices,
    std::vector<std::uint32_t> indices) noexcept
{
    std::shared_ptr<Mesh> result(new Mesh(e, std::move(vertices), std::move(indices)));
    result->self = result;
    return result;
}

gearoenix::vulkan::mesh::Mesh::~Mesh() noexcept = default;

void gearoenix::vulkan::mesh::Mesh::set_component(const std::shared_ptr<core::ecs::EntitySharedBuilder>&) noexcept
{
    GX_UNIMPLEMENTED
}

#endif