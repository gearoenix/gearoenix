#include "gx-vk-msh-mesh.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../buffer/gx-vk-buf-buffer.hpp"
#include "../buffer/gx-vk-buf-manager.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../device/gx-vk-dev-logical.hpp"
#include "../buffer/gx-vk-buf-manager.hpp"

void gearoenix::vulkan::mesh::Mesh::initialize_blas()
{
    VkAccelerationStructureDeviceAddressInfoKHR info;
    GX_SET_ZERO(info);
    info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
    info.accelerationStructure = vulkan_data;
    acceleration_address = vkGetAccelerationStructureDeviceAddressKHR(device::Logical::get().get_vulkan_data(), &info);
}

gearoenix::vulkan::mesh::Mesh::Mesh(
    const std::string& name,
    const render::Vertices& vertices,
    const std::vector<std::uint32_t>& indices,
    math::Aabb3<double>&& occlusion_box,
    const core::job::EndCaller<>& c)
    : render::mesh::Mesh(nullptr, nullptr)
    , vertex(buffer::Manager::get().create(name + "-vertices", render::get_data(vertices), core::bytes_count(vertices), c))
    , index(buffer::Manager::get().create(name + "-indices", indices, c))
{
}

gearoenix::vulkan::mesh::Mesh::~Mesh()
{
    vkDestroyAccelerationStructureKHR(device::Logical::get().get_vulkan_data(), vulkan_data, nullptr);
    vulkan_data = nullptr;
}

std::pair<VkDeviceAddress, VkDeviceAddress> gearoenix::vulkan::mesh::Mesh::get_buffers_address() const
{
    return {vertex->get_device_address(), index->get_device_address() };
}

#endif