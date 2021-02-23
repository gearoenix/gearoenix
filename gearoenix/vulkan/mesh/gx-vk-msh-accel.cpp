#include "gx-vk-msh-accel.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "gx-vk-msh-accel-component.hpp"

gearoenix::vulkan::mesh::Accel::Accel(
    engine::Engine& e,
    std::vector<math::BasicVertex> vertices,
    std::vector<std::uint32_t> indices) noexcept
    : vertex(*e.get_buffer_manager().create(vertices))
    , index(*e.get_buffer_manager().create(indices))
{
    vertices.clear();
    indices.clear();
}

std::shared_ptr<gearoenix::vulkan::mesh::Accel> gearoenix::vulkan::mesh::Accel::construct(
    engine::Engine& e,
    std::vector<math::BasicVertex> vertices,
    std::vector<std::uint32_t> indices) noexcept
{
    std::shared_ptr<Accel> result(new Accel(e, std::move(vertices), std::move(indices)));
    result->self = result;
    return result;
}

gearoenix::vulkan::mesh::Accel::~Accel() noexcept = default;

void gearoenix::vulkan::mesh::Accel::set_component(const std::shared_ptr<core::ecs::EntitySharedBuilder>& b) noexcept
{
    b->get_builder().add_component(AccelComponent(self.lock()));
}

std::pair<VkDeviceAddress, VkDeviceAddress> gearoenix::vulkan::mesh::Accel::get_buffers_address() const noexcept
{
    return {
        vertex.get_device_address(),
        index.get_device_address(),
    };
}

#endif