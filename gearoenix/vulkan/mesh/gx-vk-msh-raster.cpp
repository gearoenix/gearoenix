#include "gx-vk-msh-raster.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "gx-vk-msh-raster-component.hpp"

gearoenix::vulkan::mesh::Raster::Raster(
    std::shared_ptr<buffer::Buffer> vertex,
    std::shared_ptr<buffer::Buffer> index) noexcept
    : vertex(std::move(vertex))
    , index(std::move(index))
{
}

std::shared_ptr<gearoenix::vulkan::mesh::Raster> gearoenix::vulkan::mesh::Raster::construct(
    engine::Engine& e,
    const std::vector<math::BasicVertex>& vertices,
    const std::vector<std::uint32_t>& indices) noexcept
{
    auto& buf_mgr = e.get_buffer_manager();
    auto vertex = buf_mgr.create(vertices);
    auto index = buf_mgr.create(indices);
    std::shared_ptr<Raster> result(new Raster(std::move(vertex), std::move(index)));
    result->self = result;
    return result;
}

gearoenix::vulkan::mesh::Raster::~Raster() noexcept = default;

void gearoenix::vulkan::mesh::Raster::set_component(const std::shared_ptr<core::ecs::EntitySharedBuilder>& b) noexcept
{
    b->get_builder().add_component(RasterComponent(self.lock()));
}

#endif