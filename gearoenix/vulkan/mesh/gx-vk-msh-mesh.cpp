#include "gx-vk-msh-mesh.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../buffer/gx-vk-buf-buffer.hpp"
#include "../buffer/gx-vk-buf-manager.hpp"
#include "../engine/gx-vk-eng-engine.hpp"

void gearoenix::vulkan::mesh::Mesh::initialize_blas() noexcept
{
    VkAccelerationStructureDeviceAddressInfoKHR info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
    info.accelerationStructure = vulkan_data;
    acceleration_address = vkGetAccelerationStructureDeviceAddressKHR(
        vertex->get_allocated_memory()->get_e().get_logical_device().get_vulkan_data(), &info);
}

gearoenix::vulkan::mesh::Mesh::Mesh(
    engine::Engine& e,
    const std::string& name,
    const render::Vertices& vertices,
    const std::vector<std::uint32_t>& indices,
    math::Aabb3<double>&& occlusion_box,
    const core::sync::EndCallerIgnored& c) noexcept
    : render::mesh::Mesh(occlusion_box)
    , vertex(e.get_buffer_manager().create(name + "-vertices", render::get_data(vertices), core::bytes_count(vertices), c))
    , index(e.get_buffer_manager().create(name + "-indices", indices, c))
{
}

gearoenix::vulkan::mesh::Mesh::~Mesh() noexcept
{
    vkDestroyAccelerationStructureKHR(
        vertex->get_allocated_memory()->get_e().get_logical_device().get_vulkan_data(), vulkan_data, nullptr);
    vulkan_data = nullptr;
}

std::pair<VkDeviceAddress, VkDeviceAddress> gearoenix::vulkan::mesh::Mesh::get_buffers_address() const noexcept
{
    return {
        vertex->get_device_address(),
        index->get_device_address(),
    };
}

#endif