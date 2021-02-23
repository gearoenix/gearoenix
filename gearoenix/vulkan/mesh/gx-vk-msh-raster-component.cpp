#include "gx-vk-msh-raster-component.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "gx-vk-msh-raster.hpp"

gearoenix::vulkan::mesh::RasterComponent::RasterComponent(std::shared_ptr<Raster> raster_mesh) noexcept
    : core::ecs::Component(this)
    , raster_mesh(std::move(raster_mesh))
{
}

gearoenix::vulkan::mesh::RasterComponent::RasterComponent(RasterComponent&&) noexcept = default;

gearoenix::vulkan::mesh::RasterComponent::~RasterComponent() noexcept = default;

#endif
