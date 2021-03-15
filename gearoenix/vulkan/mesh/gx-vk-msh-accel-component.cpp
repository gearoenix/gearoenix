#include "gx-vk-msh-accel-component.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-allocator.hpp"
#include "../buffer/gx-vk-buf-buffer.hpp"
#include "gx-vk-msh-accel.hpp"

gearoenix::vulkan::mesh::AccelComponent::AccelComponent(std::shared_ptr<Accel> accel_mesh) noexcept
    : core::ecs::Component(this)
    , acceleration_structure(accel_mesh->get_vulkan_data())
    , acceleration_address(accel_mesh->get_acceleration_address())
    , acceleration_mesh(std::move(accel_mesh))
    , vertex_buffer_offset(static_cast<std::uint32_t>(acceleration_mesh->get_vertex()->get_allocator()->get_offset()))
    , vertex_buffer_size(static_cast<std::uint32_t>(acceleration_mesh->get_vertex()->get_allocator()->get_size()))
    , index_buffer_offset(static_cast<std::uint32_t>(acceleration_mesh->get_index()->get_allocator()->get_offset()))
    , index_buffer_size(static_cast<std::uint32_t>(acceleration_mesh->get_index()->get_allocator()->get_size()))
{
}

gearoenix::vulkan::mesh::AccelComponent::AccelComponent(AccelComponent&&) noexcept = default;

gearoenix::vulkan::mesh::AccelComponent::~AccelComponent() noexcept = default;

#endif
