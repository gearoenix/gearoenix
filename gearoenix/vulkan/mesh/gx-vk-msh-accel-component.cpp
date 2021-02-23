#include "gx-vk-msh-accel-component.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "gx-vk-msh-accel.hpp"

gearoenix::vulkan::mesh::AccelComponent::AccelComponent(std::shared_ptr<Accel> raster_mesh) noexcept
    : core::ecs::Component(this)
    , raster_mesh(std::move(raster_mesh))
{
}

gearoenix::vulkan::mesh::AccelComponent::AccelComponent(AccelComponent&&) noexcept = default;

gearoenix::vulkan::mesh::AccelComponent::~AccelComponent() noexcept = default;

#endif
