#include "gx-vk-msh-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-check.hpp"
#include "../gx-vk-marker.hpp"
#include "../query/gx-vk-qu-pool.hpp"
#include "gx-vk-msh-accel.hpp"
#include "gx-vk-msh-raster.hpp"

void gearoenix::vulkan::mesh::Manager::create_accel(
    const std::string& name,
    std::vector<math::BasicVertex> vertices,
    std::vector<std::uint32_t> indices,
    core::sync::EndCaller<render::mesh::Mesh>& c) noexcept
{
    const auto vertices_count = vertices.size();
    const auto indices_count = indices.size();

    auto mesh = Accel::construct(e, std::move(vertices), std::move(indices));

    auto& logical_device = e.get_logical_device();
    auto& command_manager = e.get_command_manager();
    const auto vk_device = logical_device.get_vulkan_data();

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
    bge_info.scratchData.deviceAddress = scratch_address;

    query::Pool query_pool(logical_device, VK_QUERY_TYPE_ACCELERATION_STRUCTURE_COMPACTED_SIZE_KHR);

    auto cmd = command_manager.create(command::Type::Primary);
    cmd.begin();
    cmd.build_acceleration_structure(bge_info, rng_info);

    {
        GX_GUARD_LOCK(blass_info)
        blass_info.emplace_back(geo_info, rng_info);
    }

    c.set_data(std::move(mesh));
}

void gearoenix::vulkan::mesh::Manager::create_raster(
    const std::string& name,
    std::vector<math::BasicVertex> vertices,
    std::vector<std::uint32_t> indices,
    core::sync::EndCaller<render::mesh::Mesh>& c) noexcept
{
    c.set_data(Raster::construct(e, std::move(vertices), std::move(indices)));
}

gearoenix::vulkan::mesh::Manager::Manager(engine::Engine& e) noexcept
    : e(e)
    , use_accel(e.get_logical_device().get_physical_device().get_rtx_supported())
{
}

gearoenix::vulkan::mesh::Manager::~Manager() noexcept = default;

void gearoenix::vulkan::mesh::Manager::create(
    const std::string& name,
    std::vector<math::BasicVertex> vertices,
    std::vector<std::uint32_t> indices,
    core::sync::EndCaller<render::mesh::Mesh>& c) noexcept
{
    if (use_accel) {
        create_accel(name, std::move(vertices), std::move(indices), c);
    } else {
        create_raster(name, std::move(vertices), std::move(indices), c);
    }
}

#endif