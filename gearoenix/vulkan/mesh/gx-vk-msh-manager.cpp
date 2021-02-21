#include "gx-vk-msh-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../engine/gx-vk-eng-engine.hpp"
#include "gx-vk-msh-mesh.hpp"

gearoenix::vulkan::mesh::Manager::Manager(engine::Engine& e) noexcept
    : e(e)
{
}

gearoenix::vulkan::mesh::Manager::~Manager() noexcept = default;

void gearoenix::vulkan::mesh::Manager::create(
    std::vector<math::BasicVertex> vertices,
    std::vector<std::uint32_t> indices,
    core::sync::EndCaller<render::mesh::Mesh>& c) noexcept
{
    c.set_data(Mesh::construct(e, std::move(vertices), std::move(indices)));
}
#endif