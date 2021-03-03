#include "gx-vk-msh-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-allocator.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../../core/sync/gx-cr-sync-work-waiter.hpp"
#include "../buffer/gx-vk-buf-buffer.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-check.hpp"
#include "../gx-vk-marker.hpp"
#include "../query/gx-vk-qry-pool.hpp"
#include "../queue/gx-vk-qu-queue.hpp"
#include "../sync/gx-vk-sync-fence.hpp"
#include "gx-vk-msh-accel.hpp"
#include "gx-vk-msh-raster.hpp"

void gearoenix::vulkan::mesh::Manager::create_accel(
    const std::string& name,
    const std::vector<math::BasicVertex>& vertices,
    const std::vector<std::uint32_t>& indices,
    core::sync::EndCaller<render::mesh::Mesh>& c) noexcept
{
    core::sync::EndCaller<Accel> end([this,
                                         name = name,
                                         c = c,
                                         vertices_count = vertices.size(),
                                         indices_count = indices.size()](const auto& result) mutable noexcept {
        accel_creator->push([this,
                                name = std::move(name),
                                c = std::move(c),
                                vertices_count,
                                indices_count,
                                result]() mutable noexcept {
            create_accel_after_vertices_ready(std::move(name), vertices_count, indices_count, std::move(c), std::move(result));
        });
    });
    c.set_data(Accel::construct(e, vertices, indices, end));
}

void gearoenix::vulkan::mesh::Manager::create_accel_after_vertices_ready(
    std::string name,
    const std::size_t vertices_count,
    const std::size_t indices_count,
    core::sync::EndCaller<render::mesh::Mesh> c,
    std::shared_ptr<Accel> result) noexcept
{
    auto& dev = e.get_logical_device();
    auto& cmd_mgr = e.get_command_manager();
    const auto vk_dev = dev.get_vulkan_data();

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
    trs_info.vertexStride = sizeof(math::BasicVertex);

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
        vk_dev,
        VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
        &bge_info, &rng_info.primitiveCount, &bsz_info);

    result->accel = std::move(e.get_buffer_manager().create_static(static_cast<std::size_t>(bsz_info.accelerationStructureSize)));
    GX_CHECK_NOT_EQUAL_D(nullptr, result->accel)

    VkAccelerationStructureCreateInfoKHR asc_info;
    GX_SET_ZERO(asc_info)
    asc_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
    asc_info.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    asc_info.size = bsz_info.accelerationStructureSize;
    asc_info.buffer = result->accel->get_vulkan_data();
    asc_info.offset = result->accel->get_allocator()->get_offset();
    asc_info.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;

    GX_VK_CHK(vkCreateAccelerationStructureKHR(vk_dev, &asc_info, nullptr, &result->vulkan_data))

    mark(name + "-blas", result->vulkan_data, e.get_logical_device());

    bge_info.dstAccelerationStructure = result->vulkan_data;

    auto scratch_buf = e.get_buffer_manager().create_static(static_cast<std::size_t>(bsz_info.buildScratchSize));
    GX_CHECK_NOT_EQUAL_D(nullptr, scratch_buf)
    const auto scratch_address = scratch_buf->get_device_address();
    bge_info.scratchData.deviceAddress = scratch_address;

    std::shared_ptr<query::Pool> query_pool(new query::Pool(
        dev, VK_QUERY_TYPE_ACCELERATION_STRUCTURE_COMPACTED_SIZE_KHR));

    auto cmd = std::make_shared<command::Buffer>(std::move(cmd_mgr.create(command::Type::Primary)));
    cmd->begin();
    cmd->build_acceleration_structure(bge_info, rng_info);
    cmd->end();
    std::shared_ptr<sync::Fence> fence(new sync::Fence(dev));
    dev.get_graphic_queue()->submit(*cmd, *fence);

    accel_creation_waiter->push([this,
                                    cmd = std::move(cmd),
                                    fence = std::move(fence),
                                    c = std::move(c),
                                    result = std::move(result),
                                    query_pool = std::move(query_pool)]() mutable noexcept {
        create_accel_after_blas_ready(
            std::move(cmd), std::move(fence), std::move(c), std::move(result), std::move(query_pool));
    });

    name.clear();
}

void gearoenix::vulkan::mesh::Manager::create_accel_after_blas_ready(
    std::shared_ptr<command::Buffer> creation_cmd,
    std::shared_ptr<sync::Fence> fence,
    core::sync::EndCaller<render::mesh::Mesh> c,
    std::shared_ptr<Accel> result,
    std::shared_ptr<query::Pool> query_pool) noexcept
{
    fence->wait();
    auto& dev = e.get_logical_device();
    auto& cmd_mgr = e.get_command_manager();
    auto cmd = std::make_shared<command::Buffer>(std::move(cmd_mgr.create(command::Type::Primary)));
    cmd->begin();
    query_pool->issue_acceleration_structure_compacted_size(*cmd, result->vulkan_data);
    cmd->end();
    fence = std::make_shared<sync::Fence>(dev);
    dev.get_graphic_queue()->submit(*cmd, *fence);
    accel_creation_waiter->push([this,
                                    cmd = std::move(cmd),
                                    fence = std::move(fence),
                                    query_pool = std::move(query_pool),
                                    result = std::move(result),
                                    c = std::move(c)]() mutable noexcept {
        create_accel_after_query_ready(
            std::move(cmd), std::move(fence), std::move(c), std::move(result), std::move(query_pool));
    });
    accel_creator->push([cmd = std::move(creation_cmd)] {});
}

void gearoenix::vulkan::mesh::Manager::create_accel_after_query_ready(
    std::shared_ptr<command::Buffer> query_cmd,
    std::shared_ptr<sync::Fence> fence,
    core::sync::EndCaller<render::mesh::Mesh> c,
    std::shared_ptr<Accel> result,
    std::shared_ptr<query::Pool> query_pool) noexcept
{
    VkAccelerationStructureKHR old_accel = result->vulkan_data;

    auto& dev = e.get_logical_device();
    auto& cmd_mgr = e.get_command_manager();
    const auto vk_dev = dev.get_vulkan_data();

    fence->wait();
    const auto compact_size = query_pool->get_acceleration_structure_compacted_size();

    result->accel = std::move(e.get_buffer_manager().create_static(static_cast<std::size_t>(compact_size)));
    GX_CHECK_NOT_EQUAL_D(nullptr, result->accel)

    VkAccelerationStructureCreateInfoKHR asc_info;
    GX_SET_ZERO(asc_info)
    asc_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
    asc_info.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    asc_info.size = compact_size;
    asc_info.buffer = result->accel->get_vulkan_data();
    asc_info.offset = result->accel->get_allocator()->get_offset();
    asc_info.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;

    GX_VK_CHK(vkCreateAccelerationStructureKHR(vk_dev, &asc_info, nullptr, &result->vulkan_data))

    auto cmd = std::make_shared<command::Buffer>(std::move(cmd_mgr.create(command::Type::Primary)));
    cmd->begin();
    VkCopyAccelerationStructureInfoKHR copy_info;
    GX_SET_ZERO(copy_info)
    copy_info.sType = VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_INFO_KHR;
    copy_info.src = old_accel;
    copy_info.dst = result->vulkan_data;
    copy_info.mode = VK_COPY_ACCELERATION_STRUCTURE_MODE_COMPACT_KHR;
    vkCmdCopyAccelerationStructureKHR(cmd->get_vulkan_data(), &copy_info);
    cmd->end();
    fence = std::make_shared<sync::Fence>(dev);
    dev.get_graphic_queue()->submit(*cmd, *fence);
    accel_creation_waiter->push([cmd = std::move(cmd),
                                    fence = std::move(fence),
                                    c = std::move(c)]() mutable noexcept {
        fence->wait();
    });

    query_cmd = nullptr;
    query_pool = nullptr;
    result = nullptr;
}

void gearoenix::vulkan::mesh::Manager::create_raster(
    const std::string& name,
    const std::vector<math::BasicVertex>& vertices,
    const std::vector<std::uint32_t>& indices,
    core::sync::EndCaller<render::mesh::Mesh>& c) noexcept
{
    c.set_data(Raster::construct(e, vertices, indices, core::sync::EndCallerIgnored([c] {})));
}

gearoenix::vulkan::mesh::Manager::Manager(engine::Engine& e) noexcept
    : e(e)
    , use_accel(e.get_logical_device().get_physical_device().get_rtx_supported())
    , accel_creator(new core::sync::WorkWaiter())
    , accel_creation_waiter(new core::sync::WorkWaiter())
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

#endif