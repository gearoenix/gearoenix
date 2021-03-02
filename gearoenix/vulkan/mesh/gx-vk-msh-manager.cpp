#include "gx-vk-msh-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-allocator.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../buffer/gx-vk-buf-buffer.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-check.hpp"
#include "../gx-vk-marker.hpp"
#include "../query/gx-vk-qry-pool.hpp"
#include "gx-vk-msh-accel.hpp"
#include "gx-vk-msh-raster.hpp"

void gearoenix::vulkan::mesh::Manager::create_pending_accels() noexcept
{
    decltype(pending_accel_meshes) inputs;
    {
        GX_GUARD_LOCK(pending_accel_meshes)
        std::swap(pending_accel_meshes, inputs);
    }
    const auto fun = [this](
                         const std::string& name,
                         const std::vector<math::BasicVertex>& vertices,
                         const std::vector<std::uint32_t>& indices,
                         core::sync::EndCaller<render::mesh::Mesh>& c,
                         const int attempts) noexcept {
        create_accel(name, vertices, indices, c, attempts);
    };
    for (auto& args : inputs)
        std::apply(fun, args);
}

void gearoenix::vulkan::mesh::Manager::create_accel(
    const std::string& name,
    const std::vector<math::BasicVertex>& vertices,
    const std::vector<std::uint32_t>& indices,
    core::sync::EndCaller<render::mesh::Mesh>& c,
    const int attempts) noexcept
{
    GX_CHECK_NOT_EQUAL(attempts, max_attempts)

    const auto vertices_count = vertices.size();
    const auto indices_count = indices.size();

    auto result = Accel::construct(e, vertices, indices);
    if (nullptr == result) {
        GX_GUARD_LOCK(pending_accel_meshes)
        pending_accel_meshes.emplace_back(name, vertices, indices, c, attempts + 1);
        return;
    }

    auto& logical_device = e.get_logical_device();
    auto& command_manager = e.get_command_manager();
    const auto vk_device = logical_device.get_vulkan_data();

    const auto [vba, iba] = result->get_buffers_address();

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
    trs_info.vertexStride = sizeof(std::remove_reference_t<decltype(vertices)>::value_type);

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

    result->accel = std::move(e.get_buffer_manager().create_static(static_cast<std::size_t>(bsz_info.accelerationStructureSize)));
    if (nullptr == result->accel) {
        GX_GUARD_LOCK(pending_accel_meshes)
        pending_accel_meshes.emplace_back(name, vertices, indices, c, attempts + 1);
        return;
    }

    VkAccelerationStructureCreateInfoKHR asc_info;
    GX_SET_ZERO(asc_info)
    asc_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
    asc_info.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    asc_info.size = bsz_info.accelerationStructureSize;
    asc_info.buffer = result->accel->get_vulkan_data();
    asc_info.offset = result->accel->get_allocator()->get_offset();
    asc_info.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;

    GX_VK_CHK(vkCreateAccelerationStructureKHR(vk_device, &asc_info, nullptr, &result->vulkan_data))

    mark(name + "-blas", result->vulkan_data, e.get_logical_device());

    bge_info.dstAccelerationStructure = result->vulkan_data;

    auto scratch_buf = e.get_buffer_manager().create_static(static_cast<std::size_t>(bsz_info.buildScratchSize));
    if (nullptr == scratch_buf) {
        GX_GUARD_LOCK(pending_accel_meshes)
        pending_accel_meshes.emplace_back(name, vertices, indices, c, attempts + 1);
        return;
    }
    const auto scratch_address = scratch_buf->get_device_address();
    bge_info.scratchData.deviceAddress = scratch_address;

    std::shared_ptr<query::Pool> query_pool(new query::Pool(
        logical_device, VK_QUERY_TYPE_ACCELERATION_STRUCTURE_COMPACTED_SIZE_KHR));

    c.set_data(result);
    {
        GX_GUARD_LOCK(blass_info)
        blass_info.emplace_back(
            geo_info, rng_info, bge_info, std::move(result),
            std::move(query_pool), std::move(scratch_buf), c);
    }
}

void gearoenix::vulkan::mesh::Manager::create_raster(
    const std::string& name,
    const std::vector<math::BasicVertex>& vertices,
    const std::vector<std::uint32_t>& indices,
    core::sync::EndCaller<render::mesh::Mesh>& c) noexcept
{
    c.set_data(Raster::construct(e, vertices, indices));
}

void gearoenix::vulkan::mesh::Manager::update_accel() noexcept
{
    decltype(blass_info) b_inf;
    {
        GX_GUARD_LOCK(blass_info)
        std::swap(b_inf, blass_info);
    }
    if (b_inf.empty())
        return;

    auto& frame = e.get_current_frame();
    auto& cmd_mgr = e.get_command_manager();

    for (auto& [geo_info, rng_info, bge_info, result, query_pool, scratch_buf, c] : b_inf) {
        auto cmd = cmd_mgr.create(command::Type::Primary);
        bge_info.pGeometries = &geo_info;
        cmd.begin();
        cmd.build_acceleration_structure(bge_info, rng_info);
        query_pool->issue_acceleration_structure_compacted_size(cmd, result->vulkan_data);
        cmd.end();
        //        blas_cmds.push_back(std::move(cmd));
    }
}

void gearoenix::vulkan::mesh::Manager::update_raster() noexcept
{
}

gearoenix::vulkan::mesh::Manager::Manager(engine::Engine& e) noexcept
    : e(e)
    , use_accel(e.get_logical_device().get_physical_device().get_rtx_supported())
{
}

gearoenix::vulkan::mesh::Manager::~Manager() noexcept = default;

void gearoenix::vulkan::mesh::Manager::create(
    const std::string& name,
    const std::vector<math::BasicVertex>& vertices,
    const std::vector<std::uint32_t>& indices,
    core::sync::EndCaller<render::mesh::Mesh>& c) noexcept
{
    if (use_accel) {
        create_accel(name, vertices, indices, c);
    } else {
        create_raster(name, vertices, indices, c);
    }
}

void gearoenix::vulkan::mesh::Manager::update() noexcept
{
    if (use_accel) {
        create_pending_accels();
        update_accel();
    } else {
        update_raster();
    }
}

#endif