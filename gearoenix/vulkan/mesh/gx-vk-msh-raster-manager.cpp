#include "gx-vk-msh-raster-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "gx-vk-msh-raster.hpp"

void gearoenix::vulkan::mesh::RasterManager::create_raster(
    const std::string& name,
    const std::vector<math::BasicVertex>& vertices,
    const std::vector<std::uint32_t>& indices,
    core::sync::EndCaller<render::mesh::Mesh>& c) noexcept
{
    c.set_data(Raster::construct(e, name, vertices, indices, core::sync::EndCallerIgnored([c] {})));
}

gearoenix::vulkan::mesh::RasterManager::RasterManager(engine::Engine& e) noexcept
    : Manager(e)
{
}

gearoenix::vulkan::mesh::RasterManager::~RasterManager() noexcept = default;

void gearoenix::vulkan::mesh::RasterManager::create(
    const std::string& name,
    const std::vector<math::BasicVertex>& vertices,
    const std::vector<std::uint32_t>& indices,
    core::sync::EndCaller<render::mesh::Mesh>& c) noexcept
{
    create_raster(name, vertices, indices, c);
}

void gearoenix::vulkan::mesh::RasterManager::update() noexcept
{
}

#endif