#include "gx-vk-msh-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-check.hpp"
#include "../gx-vk-marker.hpp"
#include "gx-vk-msh-mesh.hpp"

gearoenix::vulkan::mesh::Manager::Manager(engine::Engine& e) noexcept
    : e(e)
{
}

gearoenix::vulkan::mesh::Manager::~Manager() noexcept = default;

void gearoenix::vulkan::mesh::Manager::create(
    const std::string& name,
    std::vector<math::BasicVertex> vertices,
    std::vector<std::uint32_t> indices,
    core::sync::EndCaller<render::mesh::Mesh>& c) noexcept
{
    const auto vertices_count = vertices.size();
    const auto indices_count = indices.size();

    auto mesh = Mesh::construct(e, std::move(vertices), std::move(indices));

    const auto vk_device = e.get_logical_device().get_vulkan_data();

    const auto [vba, iba] = mesh->get_buffers_address();

    VkAccelerationStructureGeometryKHR geo_info;
    GX_SET_ZERO(geo_info)
    geo_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
    geo_info.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
    geo_info.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;

    auto& trs_info = geo_info.geometry.triangles;
    trs_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
    trs_info.indexData.deviceAddress = iba;
    trs_info.indexType = VK_INDEX_TYPE_UINT32;
    trs_info.maxVertex = static_cast<std::uint32_t>(vertices_count);
    trs_info.vertexData.deviceAddress = vba;
    trs_info.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
    trs_info.vertexStride = sizeof(decltype(vertices)::value_type);

    VkAccelerationStructureBuildRangeInfoKHR rng_info;
    GX_SET_ZERO(rng_info)
    rng_info.primitiveCount = static_cast<std::uint32_t>(indices_count / 3);

    VkAccelerationStructureBuildGeometryInfoKHR bge_info;
    GX_SET_ZERO(bge_info)
    bge_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    bge_info.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR | VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_KHR;
    bge_info.geometryCount = 1;
    bge_info.pGeometries = &geo_info;
    bge_info.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    bge_info.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;

    VkAccelerationStructureBuildSizesInfoKHR bsz_info;
    GX_SET_ZERO(bsz_info)
    bsz_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
    vkGetAccelerationStructureBuildSizesKHR(
        vk_device,
        VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
        &bge_info, &rng_info.primitiveCount, &bsz_info);

    auto buf = std::move(*e.get_buffer_manager().create_static(static_cast<std::size_t>(bsz_info.accelerationStructureSize)));

    VkAccelerationStructureCreateInfoKHR asc_info;
    GX_SET_ZERO(asc_info)
    asc_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
    asc_info.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    asc_info.size = bsz_info.accelerationStructureSize;
    asc_info.buffer = buf.get_vulkan_data();
    asc_info.offset = buf.get_allocator().get_offset();
    asc_info.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;

    VkAccelerationStructureKHR accel;

    GX_VK_CHK(vkCreateAccelerationStructureKHR(vk_device, &asc_info, nullptr, &accel))

    mark(name + "-blas", accel, e.get_logical_device());

    bge_info.dstAccelerationStructure = accel;

    auto scratch_buf = std::move(*e.get_buffer_manager().create_static(static_cast<std::size_t>(bsz_info.buildScratchSize)));
    const auto scratch_address = scratch_buf.get_device_address();

    {
        GX_GUARD_LOCK(blass_info)
        blass_info.emplace_back(geo_info, rng_info);
    }

    c.set_data(std::move(mesh));
}
#endif